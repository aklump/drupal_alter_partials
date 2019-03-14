<?php

namespace Drupal\alter_partials\Service;

use Drupal\Core\Url;

/**
 * Core functionality provided by the alter_partials module.
 */
class AlterPartials {

  public function getPartials(array $stack) {
    global $_alter_partials_suggestions;
    if (!is_array($_alter_partials_suggestions)) {
      $_alter_partials_suggestions = [];
    }

    $index = $this->getAllAvailablePartials();
    $return = NULL;
    $stack = array_reverse($stack);
    $list = $stack;
    foreach ($stack as $key => $filename) {
      if (isset($index[$filename])) {
        $return[] = $index[$filename];
        $list[$key] = $index[$filename];
      }
    }

    $_alter_partials_suggestions[] = [
      'stack' => $stack,
      'list' => $list,
    ];

    return $return;
  }

  /**
   * Returns an array of all available partial files.
   *
   * Scans system for all partial files and caches them.
   *
   * @return array
   *   Keys are the filenames.
   *   Values are relative paths to the partial from \Drupal::root().
   */
  public function getAllAvailablePartials() {
    static $drupal_static_fast;
    if (!isset($drupal_static_fast)) {
      $drupal_static_fast['index'] = &drupal_static(__FUNCTION__, NULL);
    }
    $index = &$drupal_static_fast['index'];
    if (!isset($index)) {
      $caching = \Drupal::config('alter_partials.settings')->get('cache');
      if ($caching) {
        if ($cache = \Drupal::cache('cache')->get('alter_partials:partials')) {
          $index = empty($cache->data) ? NULL : $cache->data;
        }
        else {
          // Set the default values.
          $index = [];
        }
      }
      if (empty($index)) {
        // Search the filesystem for directories.
        $info = \Drupal::moduleHandler()->invokeAll('alter_partials_info');
        \Drupal::moduleHandler()->alter('alter_partials_info', $info, $stack);
        $dirs = $info['directory'];
        $found = [];
        if (isset($dirs['module'])) {
          if (!is_array($dirs['module'])) {
            throw new \InvalidArgumentException("module must be an array in implementation of hook_alter_partials_info");
          }
          foreach ($dirs['module'] as $path) {
            $list = file_scan_directory($path, '/.*\.inc$/', ['recurse' => FALSE]);
            $found = array_merge($found, array_keys($list));
          }
        }
        if (isset($dirs['theme'])) {
          if (!is_array($dirs['theme'])) {
            throw new \InvalidArgumentException("theme must be an array in implementation of hook_alter_partials_info");
          }
          foreach ($dirs['theme'] as $path) {
            $list = file_scan_directory($path, '/.*\.inc$/', ['recurse' => FALSE]);
            $found = array_merge($found, array_keys($list));
          }
        }

        $index = [];
        foreach ($found as $path) {
          $index[pathinfo($path, PATHINFO_FILENAME)] = $path;
        }

        if ($caching) {
          \Drupal::cache('cache')
            ->set('alter_partials:partials', $index, \Drupal\Core\Cache\Cache::PERMANENT);
        }
      }
    }

    return $index;
  }

  /**
   * Return a stack of filepaths to check for alters for this build array.
   *
   * @param  array $build The render array
   * @param  string $type This may not be the bundle type in all cases, e.g.
   *                       'ds'.
   *
   * @return array  Files should be tested from the last to the first.
   */
  public function getEntityStack($build) {
    $stack = [];

    $type = isset($build['#entity_type']) ? $build['#entity_type'] : NULL;

    $key = NULL;
    switch ($type) {
      case 'taxonomy_term':
        $key = '#term';
        break;

      case 'node':
        $key = '#node';
        break;

      case 'user':
        $key = '#account';
        break;

      default:
        if (isset($build['#entity'])) {
          $key = '#entity';
        }
        break;
    }
    $entity = $key && isset($build[$key]) ? $build[$key] : NULL;

    if ($entity) {
      list($id, $vid, $bundle) = entity_extract_ids($build['#entity_type'], $entity);
      $view_mode = $build['#view_mode'];

      if (!empty($id)) {
        $stack = \Drupal::service('alter_partials')
          ->getFilenameStack($build['#entity_type'], $bundle, $view_mode, $id);
      }
    }
    elseif (isset($build['#alter_partials_type'])) {
      $stack = [];
      $stack[] = implode('--', [
        $build['#alter_partials_type'],
        $build['#alter_partials_category'],
      ]);
      if ($build['#alter_partials_version']) {
        $stack[] = implode('--', [
          $build['#alter_partials_type'],
          $build['#alter_partials_version'],
        ]);
        $stack[] = implode('--', [
          $build['#alter_partials_type'],
          $build['#alter_partials_category'],
          $build['#alter_partials_version'],
        ]);
      }
    }

    $stack = array_unique($stack);
    foreach ($stack as &$file) {
      $file = str_replace('_', '-', $file);
    }

    \Drupal::moduleHandler()
      ->alter('alter_partials_entity_stack', $stack, $build);

    return $stack;
  }

  /**
   * Generates a file stack for inclusions.
   *
   * @param  string $base E.g. entity type, ds, etc.
   * @param  string $type E.g. bundle name
   * @param  mixed $id E.g. entity id
   *
   * @return array
   */
  public function getFilenameStack($entity_type, $type, $viewmode, $id, $prefix = '') {
    $stack = [];

    // Least specific.
    $include = $prefix ? [$prefix, $entity_type] : [$entity_type];
    $include[] = $viewmode;
    $stack[] = implode('--', $include);

    if ($viewmode !== 'default') {
      array_pop($include);
      $include[] = 'default';
      $stack[] = implode('--', $include);
    }

    // Middle specific by bundle.
    $include = $prefix ? [$prefix, $entity_type] : [$entity_type];
    $include[] = $type;
    $include[] = $viewmode;
    $stack[] = implode('--', $include);

    if ($viewmode !== 'default') {
      array_pop($include);
      $include[] = 'default';
      $stack[] = implode('--', $include);
    }

    // Most specific includes the id.
    $include = $prefix ? [$prefix, $entity_type, $id] : [
      $entity_type,
      $id,
    ];

    $include[] = $viewmode;
    $stack[] = implode('--', $include);

    if ($viewmode !== 'default') {
      array_pop($include);
      $include[] = 'default';
      $stack[] = implode('--', $include);
    }

    return $stack;
  }

  public function getPathStack() {
    $stack = [];
    $include[] = 'page';
    $include[] = str_replace('/', '-', Url::fromRoute("<current>")->toString());
    $stack[] = implode('--', $include);

    return $stack;
  }

  /**
   * Inserts node-based vars for the alter partial.
   *
   * @param  array &$vars
   *   The variables array.
   * @param  object $node
   */
  public function addNodeVars(array &$vars, $node) {
    $vars = [
        'node' => $node,
        'entity' => $node,
        'lang' => ($l = $node->language) ? $l : language_default(),
      ] + $vars;
  }

  /**
   * Inserts view-based vars for the alter partial.
   *
   * @param  array &$vars
   *   The variables array.
   * @param  object $view
   */
  public function getViewVars(array &$vars, $view) {
    $vars = [
        'view' => $view,
        'name' => $view->name,
        'display_id' => $view->current_display,
      ] + $vars;
  }

  /**
   * Adds block-based variables.
   *
   * @param array $vars
   * @param $block
   * @param array $build
   * @param array $core_vars
   */
  public function addBlockVars(array &$vars, $block, array $build, array $core_vars) {
    $vars = [
        'block' => $block,
        'module' => $block->module,
        'delta' => $block->delta,
        'subject' => $block->subject,
        'content' => $build[0]['#markup'],
        'id' => $core_vars['block_html_id'],
        'zebra' => $core_vars['block_zebra'],
      ] + $vars;
  }

  /**
   * Inserts global vars for the alter partials.
   *
   * @param  array &$vars
   *   The variables array.
   * @param  array $build
   *   The original build array.
   */
  public function addGlobalVars(array &$vars, array $build) {
    $prop_keys = array_flip(\Drupal\Core\Render\Element::properties($build));
    $global_vars = [
      'original' => $build,
      'build' => $build,
      'elements' => array_diff_key($build, $prop_keys),
      'properties' => array_intersect_key($build, $prop_keys),
      'lang' => isset($build['#language']) ? $build['#language'] : language_default(),
    ];
    if (isset($build['#entity']) && isset($build['#entity_type'])) {
      $global_vars['entity_type_id'] = $build['#entity_type'];
      $global_vars[$build['#entity_type']] = $build['#entity'];
      $global_vars['entity'] = $build['#entity'];

      // Get the entity id.
      if (method_exists($build['#entity'], 'identifier')) {
        $global_vars['entity_id'] = $build['#entity']->identifier();
      }
      else {
        $info = \Drupal::entityManager()->getDefinition($build['#entity_type']);
        $key = isset($info['entity keys']['name']) ? $info['entity keys']['name'] : $info['entity keys']['id'];
        $global_vars['entity_id'] = isset($build['#entity']->$key) ? $build['#entity']->$key : NULL;
      }

      if (\Drupal::moduleHandler()->moduleExists('data_api')) {
        $global_vars['e'] = data_api($build['#entity_type']);
      }
    }
    if (isset($build['#view_mode'])) {
      $global_vars['view_mode'] = $build['#view_mode'];
    }
    $vars = $global_vars + $vars;
  }
}
