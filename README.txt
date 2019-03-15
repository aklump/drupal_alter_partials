                         Drupal Module: Alter Partials

   Author: Aaron Klump [1]sourcecode@intheloftstudios.com

Quick Start

    1. Enable this module.
    2. Load any node page in your browser.
    3. In your theme create a folder alter_partials/.
    4. In that folder, create node--default.inc with the following
       snippet.
<?php

$build = [
  '#markup' => 'Hello world!',
];

    5. Now reload the node page.
    6. Because you set $build to a render array to print Hello World!,
       that is used in place of the default render array.
    7. To see the default array change the contents of node--default.inc
       to something like:
<?php

use Drupal\Core\Render\Element;

print '<pre>';
print_r(array_keys($build));
print '</pre>';
die;

What Happened?

   In it's simplest form, use a partial file to alter $build and you can
   control the output of things using render arrays programatically.

   This module allows you to use partial files much the same way that you
   use tpl files in your theme, to alter build arrays. All files should be
   placed in a subfolder of your theme called alter_partials. This is in
   lieu of placing lots of changes in one big hook_HOOK_alter() function,
   which may get unruly.

   Support exists for several entities.

   By creating one of those files in your theme directory and manipulating
   the $build or $var variables, you will affect change.

   For performance reasons, you must include the view mode.

View modes field

   An extra field is provided to serve as a UI indicator that the view
   mode is not being configured via the UI, but instead via code. It reads
   Display managed in code. The intention is that for a given display
   mode, you can show only this field and it will be a clear sign to the
   content managers why they cannot configure the view mode using normal
   field means. For more info see
   hook_alter_partials_entities_in_code_alter(). * When you create a
   partial file, it should automatically cause this new view mode field to
   appear; if it doesn't look into
   hook_alter_partials_entities_in_code_alter()

Advanced Configuration

    1. It is possible for modules to provide alter partials as well. Refer
       to alter_partials.api.php for more info.

Development

     * During development you can disable the caching and thereby avoid
       having to drupal cache clear when adding new partials; you should
       not do this on production though. To do so add the following line
       to your settings.php file:
$conf['alter_partials.settings']['cache'] = false;

     * There is an included module alter_partials_dev which should not be
       enabled in a production environment.
     * The above cache disable is accomplished for you when you enable
       alter_partials_dev module, which also provides a block to help with
       filenames. Enable the module and visit the blocks admin page and
       assign it to a region. That block will populate with all the
       possible filenames that could have been used for that page
       (requires JS).

Design Decisions/Rationale

   With the move toward smaller partials like in SASS I though it would be
   easier to manage code doing things this way rather than hundreds of
   lines of a function all wrapped in node_view_alter().

Contact

     * In the Loft Studios
     * Aaron Klump - Developer
     * PO Box 29294 Bellingham, WA 98228-1294
     * skype: intheloftstudios
     * d.o: aklump
     * [2]http://www.InTheLoftStudios.com

References

   1. mailto:sourcecode@intheloftstudios.com
   2. http://www.InTheLoftStudios.com/
