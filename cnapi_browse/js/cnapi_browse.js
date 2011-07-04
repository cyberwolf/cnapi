(function ($) {
  $.dpText = {
	  TEXT_PREV_YEAR		:	Drupal.t('Previous year'),
	  TEXT_PREV_MONTH		:	Drupal.t('Previous month'),
	  TEXT_NEXT_YEAR		:	Drupal.t('Next year'),
	  TEXT_NEXT_MONTH		:	Drupal.t('Next month'),
	  TEXT_CLOSE			  :	Drupal.t('Close'),
	  TEXT_CHOOSE_DATE	:	Drupal.t('Choose date'),
	  HEADER_FORMAT		  :	'mmmm yyyy'
  };

  Drupal.behaviors.cnapiBrowseAutoSubmitForm = {
    attach: function (context, settings) {
      $('.autosubmit').change(function() {
        if ($(this).val() != '_none') {
          $(this).parents('form').submit();
          $(this).attr('disabled', true);
        }
      });
    }
  };

  Drupal.behaviors.cnapiBrowseDatePicker = {
    attach: function (context, settings) {

      // hide the date field
      $('.form-item:has(.date-for-datepicker)').hide();

      // hide the date field for fields and add the datepicker
      $('.form-item:has(.date-for-datepicker)').after('<div class="datepicker"></div>');

      // add extra option to select field to display datepicker
      $('.has-datepicker').append($("<option></option>").val("_datepicker").text(Drupal.t('Pick a date')));

      // bind onchange event to show the datepicker when the "pick a date" option has been chosen
	    $('.has-datepicker').bind('change', function () {
        if ($(this).val() == '_datepicker') {
          $('.datepicker', $(this).parents('form')).dpDisplay();

          // we need to reset the selected option so we won't submit invalid options to Drupal
          $(this).val('_none');
        }

        return false;
      });

      // create the datepicker and bind the "dateSelected" event
	    $('.datepicker')
	      .each(function () {
	        var inline = $(this).parents('form').has('.has-datepicker').length == 0;

	        $(this).datePicker({
	          createButton: false,
            displayClose: true,
            inline: inline,
            closeOnSelect: false,
            selectMultiple: true
          });
	      })
	    	.bind('dateSelected', function (e, selectedDate, $td, state) {
	    	  // when picking a date, we add or remove it from the date input field
          var selectedDates = $(this).dpGetSelected();
          var result = new Array();

          for (var i in selectedDates) {
            result.push(selectedDates[i].asString('yyyy-mm-dd'));
          }

          $('.date-for-datepicker', $(this).parents('form')).val(result.join(';'));
        });

      // highlighting selected dates in datepicker
      $('.date-for-datepicker').each(function () {
        if ($(this).val() != '') {
          var dates = $(this).val().split(';');
          for (var i in dates) {
            var date = Date.fromString(dates[i], 'yyyy-mm-dd').asString('dd/mm/yyyy');
            $('.datepicker', $(this).parents('form')).dpSetSelected(date);
          }
        }
      });
    }
  };
})(jQuery);