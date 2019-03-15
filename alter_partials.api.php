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
  return [
    'directory' => [
      'module' => [
        drupal_get_path('module', 'my_module') . '/alter_partials',
      ],
    ],
  ];
}

/**
 * Implements hook_alter_partials_entities_in_code_alter().
 *
 * $info['#available'] holds all available entity types and their bundle names,
 * so you may just want to filter on that array.
 *
 * To cause an entity to have extra fields add the entity_type as a key to
 * $info, and give the value of that new element an array of the bundles to
 * which the extra fields should apply.
 */
function hook_alter_partials_entities_in_code_alter(&$info) {
  $info['bean'] = $info['#available']['bean'];
  $info['node'] = $info['#available']['node'];
}
