<?php

namespace Drupal\Tests\alter_partials\Unit;

use AKlump\DrupalTest\UnitTestBase;
use Drupal\alter_partials\Service\AlterPartials;
use Drupal\block\Entity\Block;
use Drupal\Core\Block\BlockPluginInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\node\Entity\Node;
use Drupal\taxonomy\Entity\Term;
use Drupal\user\Entity\User;
use Drupal\views\ViewExecutable;

/**
 * Tests basic Alter Partials functionality at the Unit level.
 *
 * @group alter_partials
 */
class AlterPartialsUnitTest extends UnitTestBase {

  /**
   * Define the class being tested, it's arguments and mock objects needed.
   *
   * @var array
   *   The schema array for this test.
   */
  protected function getSchema() {
    return [
      'classToBeTested' => AlterPartials::class,
      'classArgumentsMap' => [
        'configFactory' => '\Drupal\Core\Config\ConfigFactoryInterface',
        'languageManager' => '\Drupal\Core\Language\LanguageManagerInterface',
        'entityTypeManager' => '\Drupal\Core\Entity\EntityTypeManagerInterface',
        'moduleHandler' => function () {
          $mock = \Mockery::mock('\Drupal\Core\Extension\ModuleHandlerInterface');
          $mock->allows('alter');
          return $mock;
        },
        'cache' => '\Drupal\Core\Cache\CacheBackendInterface',
      ],
      'mockObjectsMap' => [
        'language' => function () {
          $mock = \Mockery::mock('\Drupal\Core\Language\LanguageInterface');
          $mock->allows('getId')->andReturns('en');
          return $mock;
        },
        'block' => Block::class,
        'view' => ViewExecutable::class,
        'node' => Node::class,
        'user' => User::class,
        'term' => Term::class,
        'block' => BlockPluginInterface::class,
      ],
    ];
  }

  public function testStackCanBeAltered() {
    $build = ['#entity_type' => 'user', '#user' => $this->user];
    $this->args->moduleHandler = \Mockery::mock(ModuleHandlerInterface::class);
    $this->createMockedObj();
    $this->args->moduleHandler->shouldReceive('alter')
      ->once();
    $this->user->shouldReceive('id')->once()->andReturn(5);
    $this->obj->getStack($build);
  }

  public function testGetStackHandlesRandomEntityCorrectly() {
    $random_entity = \Mockery::mock();
    $random_entity->shouldReceive('id')->once()->andReturn('eyedee');
    $build = [
      '#entity_type' => 'random',
      '#entity' => $random_entity,
    ];
    $this->assertSame([
      'random--default',
      'random--eyedee--default',
    ], $this->obj->getStack($build));
  }

  public function testGetStackHandlesEmptyBuildCorrectly() {
    $build = [];
    $this->assertSame([], $this->obj->getStack($build));
  }

  public function testGetStackHandlesNonEntityCorrectly() {
    $build = [
      '#alter_partials_type' => 'alpha',
      '#alter_partials_category' => 'bravo',
      '#alter_partials_version' => 'charlie',
    ];
    $this->assertSame([
      'alpha--bravo',
      'alpha--charlie',
      'alpha--bravo--charlie',
    ], $this->obj->getStack($build));
  }

  public function testGetStackHandlesBlockCorrectly() {
    $this->block->shouldReceive('id')->once()->andReturn('bartik_page_title');
    $this->block->shouldNotReceive('getType');
    $build = [
      '#entity_type' => 'block',
      '#block' => $this->block,
      '#plugin_id' => 'page_title_block',
      '#view_mode' => 'teaser',
    ];
    $this->assertSame([
      'block--teaser',
      'block--default',
      'block--page-title-block--teaser',
      'block--page-title-block--default',
      'block--bartik-page-title--teaser',
      'block--bartik-page-title--default',
    ], $this->obj->getStack($build));
  }

  public function testGetStackHandlesTaxonomyTermCorrectly() {
    $this->term->shouldReceive('id')->once()->andReturn(814);
    $this->term->shouldNotReceive('getType');
    $build = [
      '#entity_type' => 'taxonomy_term',
      'name' => ['#object' => $this->term],
      '#view_mode' => 'teaser',
    ];
    $this->assertSame([
      'taxonomy-term--teaser',
      'taxonomy-term--default',
      'taxonomy-term--814--teaser',
      'taxonomy-term--814--default',
    ], $this->obj->getStack($build));
  }

  public function testGetStackHandlesUserCorrectly() {
    $this->user->shouldReceive('id')->once()->andReturn(55);
    $this->user->shouldNotReceive('getType');
    $build = [
      '#entity_type' => 'user',
      '#user' => $this->user,
      '#view_mode' => 'full',
    ];
    $this->assertSame([
      'user--full',
      'user--default',
      'user--55--full',
      'user--55--default',
    ], $this->obj->getStack($build));
  }

  public function testGetStackHandlesNodeCorrectly() {
    $this->node->shouldReceive('id')->once()->andReturn(123);
    $this->node->shouldReceive('getType')->once()->andReturn('person');
    $build = [
      '#entity_type' => 'node',
      '#node' => $this->node,
    ];
    $this->assertSame([
      'node--default',
      'node--person--default',
      'node--123--default',
    ], $this->obj->getStack($build));
  }

  public function testGetStackHandlesDuplicatesAndUnderscores() {
    $this->createMockedObj();
    $this->obj->shouldReceive('getFilenameStack')->once()->with(
      'node', 'person', 'default', 123
    )->andReturn([
      'alpha_bravo',
      'alpha_bravo',
      'charlie-two',
    ]);
    $this->node->shouldReceive('id')->once()->andReturn(123);
    $this->node->shouldReceive('getType')->once()->andReturn('person');
    $build = [
      '#entity_type' => 'node',
      '#node' => $this->node,
    ];
    $this->assertSame([
      'alpha-bravo',
      'charlie-two',
    ], $this->obj->getStack($build));
  }

  public function testAddGlobalVars() {
    $this->args->languageManager->shouldReceive('getDefaultLanguage')
      ->andReturn($this->language);

    $vars = ['lorem' => 'ipsum', 'node' => 'dolar'];
    $build = [
      '#alpha' => 'a',
      '#bravo' => 'b',
      'charlie' => 'c',
    ];
    $this->obj->addGlobalVars($vars, $build);
    $this->assertSame($build, $vars['original']);
    $this->assertSame($build, $vars['build']);
    $this->assertSame(['charlie' => 'c'], $vars['elements']);
    $this->assertSame(['#alpha' => 'a', '#bravo' => 'b'], $vars['properties']);
    $this->assertSame('en', $vars['lang']);
  }

  public function testAddNodeVars() {
    $vars = ['lorem' => 'ipsum', 'node' => 'dolar'];
    $this->node->shouldReceive('language')->once()->andReturn('x-default');
    $this->obj->addNodeVars($vars, $this->node);
    $this->assertSame($this->node, $vars['node']);
    $this->assertSame($this->node, $vars['entity']);
    $this->assertSame('x-default', $vars['lang']);
    $this->assertSame('ipsum', $vars['lorem']);
  }

  public function testAddViewVarsWorksAsExpected() {
    $vars = ['lorem' => 'ipsum', 'view' => 'dolar'];
    $this->view->shouldReceive('id')->once()->andReturn('footer');
    $this->view->current_display = 'default';
    $this->obj->addViewVars($vars, $this->view);
    $this->assertSame($this->view, $vars['view']);
    $this->assertSame('footer', $vars['name']);
    $this->assertSame('default', $vars['display_id']);
    $this->assertSame('ipsum', $vars['lorem']);
  }

  public function testConstruct() {
    $this->assertConstructorSetsInternalProperties();
  }

}
