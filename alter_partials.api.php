<?php
/**
 * @file
 * Defines the API functions provided by the alter_partials module.
 *
 * @ingroup alter_partials
 * @{
 */

/**
 * Implements hook_alter_partials_entity_stack_alter().
 *
 * Allow modules to add or modify the path stack produced for a single entity.
 */
function hook_alter_partials_entity_stack_alter(&$stack, $build) {
  
}

/**
 * Implements hook_alter_partials_info().
 *
 * Provide information of how your module or theme is using this module.
 *
 * @return  array
 * - directory array
 *   - theme|module Choose one for the key; 'theme' take precedence over
 *     'module' when a collision occurs.  The value should be the path to
 *     the directory containing the partial files. 
 *   
 */
function hook_alter_partials_info() {
  return array(
    'directory' => array(
      'module' => array(
        drupal_get_path('module', 'my_module') . '/alter_partials',
      )
    ),
  );
}