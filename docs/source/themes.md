---
sort: -10
---
# Quick Start

1. Create the folder _alter_partials_ in your theme.
3. Enable _alter_partials_dev_ and assign the block _Alter Partials suggestions_ to a visible region in your active theme.
4. By enabling _alter_partials_dev_ the caching of partials is disabled and you have access to a suggestion list of filenames, so this is helpful during development.
5. **_alter_partials_dev_ MUST NOT BE ENABLED ON A PRODUCTION SITE.**
6. Visit the page you want to alter and find the filename suggestion that applies.
7. Create that file inside _alter_partials_.
8. To see the variable names available in your partial use this snippet:
    
        <?php
        print '<pre>';
        print __FUNCTION__ . '(): ';
        print_r(array_keys(get_defined_vars()));
        print '</pre>';
        die;
    
1. To see an example partial file, check out _alter_partials_dev/alter_partials/node--page--full.inc_.
