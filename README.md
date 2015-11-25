# Drupal Module: Alter Partials
**Author:** Aaron Klump  <sourcecode@intheloftstudios.com>

##Summary
**Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla at massa sed nulla consectetur malesuada.**

This module allows you to use alter partials much the same way that you use tpl files in your theme.  All files should be placed in a subfolder of your theme called `alter_partials`.  The files will be searched from bottom to top and only the first file will be recognized (just like tpl files).

Support exists for several entities and display suite.

To alter display suite variables before a layout is rendered the files suggestions are the following (where we have page node with nid 17 using view mode of narrow_page):

    alter_partials/ds--node--page.inc
    alter_partials/ds--node--page--narrow-page.inc
    alter_partials/ds--node--17.inc
    alter_partials/ds--node--17--narrow-page.inc


##Requirements

##Installation
1. Install as usual, see [http://drupal.org/node/70151](http://drupal.org/node/70151) for further information.
1. You may need to modify the include path to loft_testing in `tests/phpunit/composer.json` depending upon where your module is installed relative loft testing.
1. Visit testing/phpunit and do a `composer dumpautoload`.


##Configuration

##Suggested Use

## Design Decisions/Rationale

## Testing
Testing requires the following module: http://www.intheloftstudios.com/packages/drupal/loft_testing

##Contact
* **In the Loft Studios**
* Aaron Klump - Developer
* PO Box 29294 Bellingham, WA 98228-1294
* _skype_: intheloftstudios
* _d.o_: aklump
* <http://www.InTheLoftStudios.com>