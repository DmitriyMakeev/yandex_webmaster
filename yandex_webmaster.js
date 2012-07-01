(function($) {
  var modalWindow = {
    parent: "body",
    windowId: null,
    content: null,
    open:function() {
      $(this.parent).append('<div class="modal-overlay"></div><div id="' + this.windowId + '" class="modal-window">' + this.content + '<a class="close-window"></a></div>');
      $(".close-window").click(function() {
        modalWindow.close();
      });
      $(".modal-overlay").click(function() {
        modalWindow.close();
      });
    },
    close: function() {
      $(".modal-window").remove();
      $(".modal-overlay").remove();
    }
  };
	Drupal.behaviors.initYandexWebmaster = {
		attach : function() {
      $("input.modal").click(function() {
        modalWindow.windowId = "yandex-webmaster-modal";
        modalWindow.content = '<iframe width="480" height="440" frameborder="0" scrolling="no" allowtransparency="false" src="' + $(this).attr('rel') + '"></iframe>';
        modalWindow.content += '<hr /><ol><li>';
        modalWindow.content += Drupal.t('Log in to your Yandex account.');
        modalWindow.content += '</li><li>';
        modalWindow.content += Drupal.t('Select "OK" to get security code.');
        modalWindow.content += '</li><li>';
        modalWindow.content += Drupal.t('Copy verification code, paste code to your site and save settings.');
        modalWindow.content += '</li></ol>';
        modalWindow.open();
        return false;
      });
      $("#yandex-webmaster-config-form table .form-checkbox").change(function() {
        $("#yandex-webmaster-config-form table .form-checkbox").attr('checked', false);
        $(this).attr('checked', true);
      });

      /**
      * Modified table sort by Arvind Bhardwaj (bhardwajs.on.height@gmail.com)
      * http://www.webspeaks.in/2010/09/simplest-table-sorter-with-jquery.html
      **/
      var alternateRowColors = function($table) {
        $('tbody tr:odd', $table).removeClass('even').addClass('odd');
        $('tbody tr:even', $table).removeClass('odd').addClass('even');
      };
      $('table').each(function() {
        var $table = $(this);
        $table.addClass('sortable');
        alternateRowColors($table);
        $('th', $table).each(function(column) {
          if ($(this)) {
            $(this).hover(function() {
              $(this).addClass('hover');
            }, function(){
              $(this).removeClass('hover');
            }).click(function() {
              $('th', $table).removeClass('sorted');
              $(this).addClass('sorted');
              var sortOrder = $table.data('order');
              var rows = $table.find('tbody > tr').get();
              if (sortOrder == 'd' || sortOrder == '') {
                $table.data('order','a');
                rows.sort(function(a, b) {
                  var keyA = parseInt($(a).children('td').eq(column).text().replace(' ', ''), 10);
                  var keyB = parseInt($(b).children('td').eq(column).text().replace(' ', ''), 10);
                  if (keyA < keyB) {
                    return 1;
                  }
                  if (keyA > keyB) {
                    return -1;
                  }
                  keyA = $(a).children('td').eq(column).text().toUpperCase();
                  keyB = $(b).children('td').eq(column).text().toUpperCase();
                  if (keyA < keyB) {
                    return 1;
                  }
                  if (keyA > keyB) {
                    return -1;
                  }
                  return 0;
                });
              }
              else {
                $table.data('order','d');	
                rows.sort(function(a, b) {
                  var keyA = parseInt($(a).children('td').eq(column).text().replace(' ', ''), 10);
                  var keyB = parseInt($(b).children('td').eq(column).text().replace(' ', ''), 10);
                  if (keyA < keyB) {
                    return -1;
                  }
                  if (keyA > keyB) {			
                    return 1;
                  }
                  keyA = $(a).children('td').eq(column).text().toUpperCase();
                  keyB = $(b).children('td').eq(column).text().toUpperCase();
                  if (keyA < keyB) {
                    return -1;
                  }
                  if (keyA > keyB) {			
                    return 1;
                  }
                  return 0;
                });
              }
              $.each(rows, function(index, row) {
                $table.children('tbody').append(row);
              });
              alternateRowColors($table);
            });
          }
        });
      });
		}
	}
})(jQuery);
