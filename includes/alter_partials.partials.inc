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
  return [
    'directory' => [
      'theme' => [
        drupal_get_path('theme', variable_get('theme_default', '')) . '/alter_partials',
      ],
    ],
  ];
}

/**
 * Returns the most-specific file from a path stack.
 *
 * @param  array $stack A list of paths to check for.
 * @param array &$discovered Pass an empty array to be filled by all discovered
 *   partials.
 *
 * @return array The full system paths to the include files.
 *
 * @throws InvalidArgumentException
 */
function _alter_partials_get_partials($stack, &$discovered = NULL) {
  global $_alter_partials_suggestions;
  if (!is_array($_alter_partials_suggestions)) {
    $_alter_partials_suggestions = [];
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
        // Set the default values.
        $index = [];
      }
    }
    if (empty($index)) {
      // Search the filesystem for directories.
      $info = module_invoke_all('alter_partials_info');
      drupal_alter('alter_partials_info', $info, $stack);
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
        cache_set('alter_partials:partials', $index, 'cache', CACHE_PERMANENT);
      }
    }
  }

  $discovered = $index;
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
 * Inserts global vars for the alter partials.
 *
 * @param  array &$vars
 *   The variables array.
 * @param  array $build
 *   The original build array.
 */
function _alter_partials_global_vars(&$vars, $build) {
  $prop_keys = array_flip(element_properties($build));
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
      $info = entity_get_info($build['#entity_type']);
      $key = isset($info['entity keys']['name']) ? $info['entity keys']['name'] : $info['entity keys']['id'];
      $global_vars['entity_id'] = isset($build['#entity']->$key) ? $build['#entity']->$key : NULL;
    }

    if (module_exists('data_api')) {
      $global_vars['e'] = data_api($build['#entity_type']);
    }
  }
  if (isset($build['#view_mode'])) {
    $global_vars['view_mode'] = $build['#view_mode'];
  }
  $vars = $global_vars + $vars;
}

/**
 * Inserts node-based vars for the alter partial.
 *
 * @param  array &$vars
 *   The variables array.
 * @param  object $node
 */
function _alter_partials_node_vars(&$vars, $node) {
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
function _alter_partials_view_vars(&$vars, $view) {
  $vars = [
      'view' => $view,
      'name' => $view->name,
      'display_id' => $view->current_display,
    ] + $vars;
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
function _alter_partials_entity_stack($build) {
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
      $stack = _alter_partials_filename_stack($build['#entity_type'], $bundle, $view_mode, $id);
    }
  }
  elseif (isset($build['#alter_partials_type'])) {
    $stack = [];
    //    $stack[] = $build['#alter_partials_type'];
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

  drupal_alter('alter_partials_entity_stack', $stack, $build);

  return $stack;
}

function _alter_partials_path_stack() {
  $stack = [];
  $include[] = 'page';
  $include[] = str_replace('/', '-', current_path());
  $stack[] = implode('--', $include);

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
  $stack = [];

  // Least specific
  $include = $prefix ? [$prefix, $entity_type] : [$entity_type];

  //  $stack[] = implode('--', $include);

  $include[] = $viewmode;
  $stack[] = implode('--', $include);

  if ($viewmode !== 'default') {
    array_pop($include);
    $include[] = 'default';
    $stack[] = implode('--', $include);
  }

  // Middle specific by bundle
  $include = $prefix ? [$prefix, $entity_type] : [$entity_type];

  $include[] = $type;
  //  $stack[] = implode('--', $include);

  $include[] = $viewmode;
  $stack[] = implode('--', $include);

  if ($viewmode !== 'default') {
    array_pop($include);
    $include[] = 'default';
    $stack[] = implode('--', $include);
  }

  // Most specific includes the id
  $include = $prefix ? [$prefix, $entity_type, $id] : [
    $entity_type,
    $id,
  ];

  //  $stack[] = implode('--', $include);

  $include[] = $viewmode;
  $stack[] = implode('--', $include);

  if ($viewmode !== 'default') {
    array_pop($include);
    $include[] = 'default';
    $stack[] = implode('--', $include);
  }

  return $stack;
}

/**
 * Inserts block-based vars for the alter partial.
 *
 * @param  array &$vars
 * @param  object $block
 */
function _alter_partials_block_vars(&$vars, $block, $build, $core_vars) {
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
 * Renders a theme using the system fallback when an alter partial is not found.
 *
 * @param string $hook
 *   The theme hook name.
 * @param array $variables
 *
 * @return string
 *   The rendered output.
 *
 * @see \theme from which this was taken.
 */
function _alter_partials_theme_fallback($hook, array $variables = []) {
  global $theme_path;

  // Custom insert.
  $registry = theme_get_registry();
  $info = $registry[$hook];
  $info['template'] = $info['alter_partials']['template'];
  unset($info['alter_partials']['template']);
  unset($info['function']);
  unset($info['alter_partials']);
  // End custom insert.

  $temp = $theme_path;
  // point path_to_theme() to the currently used theme path:
  $theme_path = $info['theme path'];

  // Generate the output using either a function or a template.
  $output = '';

  // Default render function and extension.
  $render_function = 'theme_render_template';
  $extension = '.tpl.php';

  // The theme engine may use a different extension and a different renderer.
  global $theme_engine;
  if (isset($theme_engine)) {
    if ($info['type'] != 'module') {
      if (function_exists($theme_engine . '_render_template')) {
        $render_function = $theme_engine . '_render_template';
      }
      $extension_function = $theme_engine . '_extension';
      if (function_exists($extension_function)) {
        $extension = $extension_function();
      }
    }

    // In some cases, a template implementation may not have had
    // template_preprocess() run (for example, if the default implementation is
    // a function, but a template overrides that default implementation). In
    // these cases, a template should still be able to expect to have access to
    // the variables provided by template_preprocess(), so we add them here if
    // they don't already exist. We don't want to run template_preprocess()
    // twice (it would be inefficient and mess up zebra striping), so we use the
    // 'directory' variable to determine if it has already run, which while not
    // completely intuitive, is reasonably safe, and allows us to save on the
    // overhead of adding some new variable to track that.
    if (!isset($variables['directory'])) {
      $default_template_variables = [];
      template_preprocess($default_template_variables, $hook);
      $variables += $default_template_variables;
    }

    // Render the output using the template file.
    $template_file = $info['template'] . $extension;
    if (isset($info['path'])) {
      $template_file = $info['path'] . '/' . $template_file;
    }
    if (variable_get('theme_debug', FALSE)) {
      $output = _theme_render_template_debug($render_function, $template_file, $variables, $extension);
    }
    else {
      $output = $render_function($template_file, $variables);
    }
  }

  // restore path_to_theme()
  $theme_path = $temp;

  return $output;
}
