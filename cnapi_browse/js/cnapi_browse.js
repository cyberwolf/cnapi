(function ($) {
  $.dpText = {
	  TEXT_PREV_YEAR		:	'Vorig jaar',
	  TEXT_PREV_MONTH		:	'Vorige maand',
	  TEXT_NEXT_YEAR		:	'Volgend jaar',
	  TEXT_NEXT_MONTH		:	'Volgende maand',
	  TEXT_CLOSE			  :	'Sluiten',
	  TEXT_CHOOSE_DATE	:	'Kies datum',
	  HEADER_FORMAT		  :	'mmmm yyyy'
  };

  Drupal.behaviors.cnapiBrowseDatePicker = {
    attach: function (context, settings) {
      // hide the date field
      $('.has-datepicker').each(function () {
        var sel = '.form-item-' + $(this).attr('id') + '-date';
        sel = sel.replace('form-item-edit', 'form-item');
        $(sel).hide();
      });
      
      // add the datepicker
      $('.form-item:has(.has-datepicker)').each(function () {
        $(this).append('<div rel="' + $('select', this).attr('id') + '" class="date-picker"></div>');
      });

      // add extra option to select field to display datepicker
      $('.has-datepicker').append($("<option></option>").val("_datepicker").text(Drupal.t('Pick a date')));

      // bind onchange event to show the datepicker when the "pick a date" option has been chosen
	    $('.has-datepicker').bind('change', function () {
        if ($(this).val() == '_datepicker') {
          $('.date-picker[rel=' + $(this).attr('id') + ']').dpDisplay();
        }
        
        // we need to reset the selected option so we won't submit invalid options to Drupal
        $(this).val('_none');
        
        return false;
      });

      // create the datepicker and bind the "dateSelected" event
	    $('.date-picker')
	      .datePicker({
	        createButton: false,
          displayClose: true,
          closeOnSelect: false,
          selectMultiple: true
        })
	    	.bind('dateSelected', function (e, selectedDate, $td, state) {
	    	  // when picking a date, we add or remove it from the date input field
          var selectedDates = $(this).dpGetSelected();
          var result = new Array();

          for (var i in selectedDates) {
            result.push(selectedDates[i].asString('yyyy-mm-dd'));
          }

          $('#' + $(this).attr('rel') + '-date').val(result.join(';'));
        });
      }
  };
})(jQuery);