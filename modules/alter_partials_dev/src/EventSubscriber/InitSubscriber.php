<?php /**
 * @file
 * Contains \Drupal\alter_partials_dev\EventSubscriber\InitSubscriber.
 */

namespace Drupal\alter_partials_dev\EventSubscriber;

use Symfony\Component\EventDispatcher\EventSubscriberInterface;
use Symfony\Component\HttpKernel\KernelEvents;

class InitSubscriber implements EventSubscriberInterface {

  /**
   * {@inheritdoc}
   */
  public static function getSubscribedEvents() {
    return [KernelEvents::REQUEST => ['onEvent', 0]];
  }

  public function onEvent() {
    global $conf;
    // Turn this off when the dev module is enabled.
    $conf['alter_partials.settings']['cache'] = FALSE;
  }

}
