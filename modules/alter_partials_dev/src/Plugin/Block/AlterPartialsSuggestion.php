<?php

namespace Drupal\alter_partials_dev\Plugin\Block;

use Drupal\Core\Block\BlockBase;

/**
 * @Block (
 *   id = "alter_partials_suggestions",
 *   admin_label = @Translation("Alter Partials Filename Suggestions"),
 *   category = @Translation("Development"),
 * )
 */
class AlterPartialsSuggestion extends BlockBase {

  /**
   * {@inheritdoc}
   */
  public function build() {
    global $_alter_partials_suggestions;
    $build = [
      '#markup' => '<div class="alter-partials-dev__suggestions"></div>',
      '#attached' => [
        'library' => ['alter_partials_dev/alter_partials_dev'],
        'drupalSettings' => [
          'alterPartialsDev' => ['suggestions' => $_alter_partials_suggestions],
        ],
      ],
      '#cache' => ['contexts' => ['route.name']],
    ];

    return $build;
  }

}
