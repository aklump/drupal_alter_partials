# Drupal Module: Alter Partials
**Author:** Aaron Klump  <sourcecode@intheloftstudios.com>

##Summary
**Entity alters using partial files not functions.**

This module allows you to use alter partials much the same way that you use tpl files in your theme.  All files should be placed in a subfolder of your theme called `alter_partials`.

Support exists for several entities and display suite.

As an example, to alter display suite layout variables before they are rendered, the files suggestions are the following (where we have page node with nid 17 using view mode of narrow_page):

Pattern is: `PREFIX--BUNDLE OR ID(--DISPLAY MODE)`.

    alter_partials/ds--node--page.inc
    alter_partials/ds--node--page--narrow-page.inc
    alter_partials/ds--node--17.inc
    alter_partials/ds--node--17--narrow-page.inc

By creating one of those files in your theme directory and manipulating the `$build` or `$var` variables, you will affect change.

## Installation
1. Install as usual, see [http://drupal.org/node/70151](http://drupal.org/node/70151) for further information.
1. Create a folder in the default theme called `alter_partials` into which you create your partial files.
1. Add one or more partial files using the idea above.  For an example look at `alter_partials/node--page--full.inc`.
1. Using the optional develop module, you can place a helper block called "Alter Partials suggestions" during development to help you know what files to use on a page.

## Advanced Configuration
1. It is possible for modules to provide alter partials as well.  Refer to `alter_partials.api.php` for more info.

## Usage

| Type | Prefix | Vars |
|----------|----------|----------|
| Node | `node--` | &$build, $node |
| User | `user--` | &$build |
| Taxonomy Term | `taxonomy-term--` | &$build |
| Display Suite | `ds--ENTITY TYPE` | &$build, &$vars*, $node |

\* `$vars`: These are the variables from node_preprocess, before node_process.

## Development
* During development you can disable the caching and thereby avoid having to drupal cache clear when adding new partials; you should not do this on production though.  To do so add the following line to your settings.php file:

        $conf['alter_partials_cache_enabled'] = FALSE;

* The above is accomplished for you when you enable `alter_partials_dev` module, which also provides a block to help with filenames.  Enable the module and visit the blocks admin page and assign it to a region.  That block will populate with all the possible filenames that could have been used for that page (requires JS).

## Design Decisions/Rationale
With the move toward smaller partials like in SASS I though it would be easier to manage code doing things this way rather than hundreds of lines of a function all wrapped in node_view_alter().


##Contact
* **In the Loft Studios**
* Aaron Klump - Developer
* PO Box 29294 Bellingham, WA 98228-1294
* _skype_: intheloftstudios
* _d.o_: aklump
* <http://www.InTheLoftStudios.com>