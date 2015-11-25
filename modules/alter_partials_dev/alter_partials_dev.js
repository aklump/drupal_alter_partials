/**
 * @file
 * The main javascript file for alter_partials_dev
 *
 * @ingroup alter_partials_dev
 */
(function ($, Drupal, window, document, undefined) {
  "use strict";

  Drupal.behaviors.alterPartialsDev = {};
  Drupal.behaviors.alterPartialsDev.attach = function (context, settings) {
    var $target = $('#block-alter-partials-dev-suggestions .content');
    if (typeof settings.alterPartialsDev.suggestions !== 'undefined' && $target.length) {
      var html = [];
      for (var i in settings.alterPartialsDev.suggestions) {
        var output = Drupal.theme('itemList', {
          type: 'ol',
          items: settings.alterPartialsDev.suggestions[i].list
        });
        html.push(output);
        var active = settings.alterPartialsDev.suggestions[i].active ? Drupal.t('Active file: ') + settings.alterPartialsDev.suggestions[i].active : Drupal.t('-- No active file --');
        html.push(Drupal.theme('placeholder', active));
      }
      $target.html(html.join("\n"));
    }
  };

  Drupal.theme.prototype.itemList = function(vars) {
    console.log(vars);
    vars = $.extend({}, {
      type: 'ul',
      items: []
    }, vars);

    var build = [];
    build.push('<' + vars.type + '>');
    for (var i in vars.items) {
      build.push('<li>' + vars.items[i] + '</li>');
    }
    build.push('</' + vars.type + '>');

    return build.join("\n");
  };

})(jQuery, Drupal, window, document, undefined);
