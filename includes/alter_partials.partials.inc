<?php
/**
 * @file
 * Provides the bulk of the work of the module.
 *
 * @ingroup alter_partials
 */

/**
 * Returns the most-specific file from a path stack.
 *
 * @param  array $stack A list of paths to check for.
 *
 * @return string  The full system path to the include file.
 */
function _alter_partials_get_partial($stack) {
  //@todo Figure out the fastest way to cache this to reduce file reads.
  while (($include = array_pop($stack))) {
    if (file_exists($include)) {
      return $include;
    }
  }

  return NULL;
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
      $stack = _alter_partials_path_stack($stack);
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

function _alter_partials_path_stack(Array $filename_stack) {
  $base = rtrim(DRUPAL_ROOT, '/') . '/' . path_to_theme() . '/alter_partials/';
  foreach ($filename_stack as &$file) {
    $file = rtrim($base, '/') . "/{$file}.inc";
  }

  return $filename_stack;
}
