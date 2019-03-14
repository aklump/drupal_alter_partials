# Quick Start

1. Create the folder _alter_partials_ in your theme.
2. Enable _alter_partials_dev_ and assign the block _Alter Partials suggestions_ to a visible region in your active theme.
3. By enabling _alter_partials_dev_ the caching of partials is disabled, you have access to a suggestion list of filenames as a block, and a partial stub writer is available, so this is very helpful during development.
4. **_alter_partials_dev_ MUST NOT BE ENABLED ON A PRODUCTION SITE.**
5. Visit the page you want to alter and find the filename suggestion that applies.
6. Create that file inside _alter_partials_.
7. Set the contents of the file to the following, and make sure the file is writeable by PHP.

        <?php
        alter_partials_dev_stub(__FILE__, get_defined_vars());
        
8. Visit the page and the file will be rewritten with a stub; reload the file to see the new contents.
9. The render array is called `$build`, which controls the output. 
10. You now have an alter partial file; modify `$build` as desired.
