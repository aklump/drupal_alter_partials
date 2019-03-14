/**
 * @file
 * The main javascript file for alter_partials_dev
 *
 * @ingroup alter_partials_dev
 */
(function($, Drupal) {
  'use strict';

  Drupal.behaviors.alterPartialsDev = {};
  Drupal.behaviors.alterPartialsDev.attach = function(context, settings) {
    var $target = $('#block-alter-partials-dev-suggestions .content');
    if (
      typeof settings.alterPartialsDev.suggestions !== 'undefined' &&
      $target.length
    ) {
      var html = [];
      for (var i in settings.alterPartialsDev.suggestions) {
        var items = settings.alterPartialsDev.suggestions[i].list.map(function(
            item
          ) {
            return item + '.inc';
          }),
          output = Drupal.theme('itemList', {
            type: 'ol',
            items: items,
          });
        html.push(output);
      }
      $target.html(html.join('\n'));
    }
  };

  Drupal.theme.prototype.itemList = function(vars) {
    vars = $.extend(
      {},
      {
        type: 'ul',
        items: [],
      },
      vars
    );

    var build = [];
    build.push('<' + vars.type + '>');
    for (var i in vars.items) {
      build.push('<li>' + vars.items[i] + '</li>');
    }
    build.push('</' + vars.type + '>');

    return build.join('\n');
  };
})(jQuery, Drupal);
