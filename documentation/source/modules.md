# Provide Partials by Module

1. Your modules can also provide alter partials but they must declare themselves using _hook_alter_partials_info_.  See _alter_partials.api.php_ for documentation.
1. Implement _hook_alter_partials_info_ in your module.
2. Follow the [theme instructions](themes.html), except that you will create that files inside your module's _alter_partials_ directory.
    
