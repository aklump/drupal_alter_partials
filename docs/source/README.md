---
title: README
sort: 100
---
# Drupal Module: Alter Partials

**Author:** Aaron Klump  <sourcecode@intheloftstudios.com>

##Summary

**Entity alters using partial files not functions.**

This module allows you to use partial files much the same way that you use tpl files in your theme, to alter build arrays.  All files should be placed in a subfolder of your theme called `alter_partials`.  This is in lieu of placing lots of changes in one big `hook_HOOK_alter()` function, which may get unruly.

Support exists for several entities and Display Suite.

As an example, to alter display suite layout variables before they are rendered, the files suggestions are the following (where we have page node with nid 17 using view mode of narrow_page):

Pattern is: `PREFIX--BUNDLE OR ID--DISPLAY MODE`.

    alter_partials/ds--node--page--narrow-page.inc
    alter_partials/ds--node--17--narrow-page.inc

By creating one of those files in your theme directory and manipulating the `$build` or `$var` variables, you will affect change.

For performance reasons, you must include the view mode.

## Installation

1. Install as usual, see [http://drupal.org/node/70151](http://drupal.org/node/70151) for further information.
2. Enable advanced help for more information and examples.

## View modes field

An extra field is provided to serve as a UI indicator that the view mode is not being configured via the UI, but instead via code.  It reads _Display managed in code_.  The intention is that for a given display mode, you can show only this field and it will be a clear sign to the content managers why they cannot configure the view mode using normal field means.  For more info see `hook_alter_partials_entities_in_code_alter()`.

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

* There is an included module `alter_partials_dev` which should not be enabled in a production environment.
* The above cache disable is accomplished for you when you enable `alter_partials_dev` module, which also provides a block to help with filenames.  Enable the module and visit the blocks admin page and assign it to a region.  That block will populate with all the possible filenames that could have been used for that page (requires JS).

## Design Decisions/Rationale

With the move toward smaller partials like in SASS I though it would be easier to manage code doing things this way rather than hundreds of lines of a function all wrapped in node_view_alter().


##Contact
* **In the Loft Studios**
* Aaron Klump - Developer
* PO Box 29294 Bellingham, WA 98228-1294
* _skype_: intheloftstudios
* _d.o_: aklump
* <http://www.InTheLoftStudios.com>
