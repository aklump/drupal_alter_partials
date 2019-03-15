---
Test Case ID: man-in-code
Author: Aaron Klump
Created: March 15, 2019
---
## Test Scenario

{{ _label }} automatically appears when you create an alter partial file.

## Pre-Conditions

1. If it doesn't already exist, create a node type _page_.

## Test Data

    _label: Display managed in code

## Test Execution

1. Load the [Manage Display admin page for page nodes](/admin/structure/types/manage/page/display).
    - Assert there is no field visible called {{ _label }}.
1. In the active theme, in the filesystem, create a directory _alter\_partials_.
1. In that directory create a file called _node--page--default.inc_
1. Rebuild caches.
1. Refresh the [Manage Display admin page for page nodes](/admin/structure/types/manage/page/display).
    - Assert you now see a field called {{ _label }}.
1. Visit another node type manage display page.
    - Assert you do not see a field called {{ _label }}.
1. Return to the [Manage Display admin page for page nodes](/admin/structure/types/manage/page/display).    
1. Delete _node--page--default.inc_
1. Rebuild caches.
    - Assert you no longer see the field called {{ _label }}.
1. In that directory now create a file called _node--teaser.inc_
1. Rebuild caches.
    - Assert you once again see the field called {{ _label }}.
1. Visit another node type manage display page.
    - Assert this time you DO see a field called {{ _label }}.
