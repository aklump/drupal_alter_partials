<?php
/**
 * @file
 * Provides render array alter to page nodes in full display mode.
 *
 * Available variables:
 * - &$build array This is the variable to modify.
 * - $elements array All elements without the property keys.
 * - $properties array An array of the properities only from $build.
 * - $view_mode string
 *
 * Node variables:
 * - $node
 * - $lang
 */

// Adds a new item list to the build array.
$build[] = array(
  '#theme' => 'item_list',
  '#items' => array('do', 're', 'mi'),
);
