<?php
/**
 * @file
 * Provides the bulk of the work of the module.
 *
 * @ingroup alter_partials
 */

/**
 * Implements hook_alter_partials_info().
 */
function alter_partials_alter_partials_info() {
  return array(
    'directory' => array(
      'theme' => array(
        drupal_get_path('theme', variable_get('theme_default', '')) . '/alter_partials',
      )
    ),
  );
}

/**
 * Returns the most-specific file from a path stack.
 *
 * @param  array $stack A list of paths to check for.
 *
 * @return array The full system paths to the include files.
 */
function _alter_partials_get_partials($stack) {
  global $_alter_partials_suggestions;
  if (!is_array($_alter_partials_suggestions)) {
    $_alter_partials_suggestions = array();
  }

  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['index'] = &drupal_static(__FUNCTION__, NULL);
  }
  $index = &$drupal_static_fast['index'];
  if (!isset($index)) {

    $caching = variable_get('alter_partials_cache_enabled', ALTER_PARTIALS_CACHE_ENABLED);
    if ($caching) {
      if ($cache = cache_get('alter_partials:partials', 'cache')) {
        $index = empty($cache->data) ? NULL : $cache->data;
      }
      else {
        // Set the default values
        $index = array();
      }
    }
    if (empty($index)) {

      // Search the filesystem for directories.
      $info  = module_invoke_all('alter_partials_info');
      $dirs  = $info['directory'];
      $found = array();
      foreach ($dirs['module'] as $path) {
        $list = file_scan_directory($path, '/.*\.inc$/', array('recurse' => FALSE));
        $found = array_merge($found, array_keys($list));
      }
      foreach ($dirs['theme'] as $path) {
        $list = file_scan_directory($path, '/.*\.inc$/', array('recurse' => FALSE));
        $found = array_merge($found, array_keys($list));
      } 
      
      $index = array();
      foreach ($found as $path) {
        $index[pathinfo($path, PATHINFO_FILENAME)] = $path;
      }
    
      if ($caching) {
        cache_set('alter_partials:partials', $index, 'cache', CACHE_PERMANENT);
      }
    }
  }

  $return = NULL;
  $stack  = array_reverse($stack);
  $list = $stack;
  foreach ($stack as $key => $filename) {
    if (isset($index[$filename])) {
      $return[]   = $index[$filename];
      $list[$key] = $index[$filename];
    }
  }

  $_alter_partials_suggestions[] = array(
    'stack' => $stack,
    'list' => $list,
  );

  return $return;
}

/**
 * Inserts global vars for the alter partials
 *
 * @param  array &$vars
 * @param  array $build
 */
function _alter_partials_global_vars(&$vars, $build) {
  $prop_keys = array_flip(element_properties($build));
  $vars = array(
    'build' => $build,
    'elements' => array_diff_key($build, $prop_keys),
    'properties' => array_intersect_key($build, $prop_keys),
  ) + $vars;
}

/**
 * Inserts node-based vars for the alter partial.
 *
 * @param  array &$vars
 * @param  object $node
 */
function _alter_partials_node_vars(&$vars, $node) {
  $vars = array(
    'node' => $node,
    'lang' => ($l = $node->language) ? $l : language_default(),
  ) + $vars;
}

/**
 * Return a stack of filepaths to check for alters for this build array.
 *
 * @param  array $build The render array
 * @param  string $type This may not be the bundle type in all cases, e.g. 'ds'.
 *
 * @return array  Files should be tested from the last to the first.
 */
function _alter_partials_entity_stack($build) {
  $stack = array();
  
  switch ($build['#entity_type']) {
    case 'taxonomy_term':
      $key = '#term';
      break;
    case 'node':
      $key = '#node';
      break;
    case 'user':
      $key = '#account';
      break;
  }
  $entity = isset($build[$key]) ? $build[$key] : NULL;
  
  if ($entity) {
    list($id, $vid, $bundle) = entity_extract_ids($build['#entity_type'], $entity);
    $view_mode               = $build['#view_mode'];

    if (!empty($id)) {
      $stack = _alter_partials_filename_stack($build['#entity_type'], $bundle, $view_mode, $id);
      // $stack = _alter_partials_path_stack($stack);
    }
  }
  drupal_alter('alter_partials_entity_stack', $stack, $build);

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
function _alter_partials_filename_stack($entity_type, $type, $viewmode, $id, $prefix = '') {
  $stack     = array();
  $include   = $prefix ? array($prefix, $entity_type) : array($entity_type);

  $include[] = $type;
  $stack[]   = implode('--', $include);
  
  $include[] = $viewmode;
  $stack[]   = implode('--', $include);

  // Now start over with the nid.
  $include   = $prefix ? array($prefix, $entity_type, $id) : array($entity_type, $id);

  $stack[]   = implode('--', $include);

  $include[] = $viewmode;
  $stack[]   = implode('--', $include);

  foreach ($stack as &$file) {
    $file = str_replace('_', '-', $file);
  }

  return $stack;
}
