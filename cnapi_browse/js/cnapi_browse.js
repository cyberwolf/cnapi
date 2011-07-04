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
      $('.form-item:has(.has-datepicker)').append('<div id="edit-when-datepicker" class="date-picker"></div>');

      $('.has-datepicker').append($("<option></option>").val("_datepicker").text(Drupal.t('Pick a date')));

	    $('.has-datepicker')
        .bind('change', function () {
          if ($(this).val() == '_datepicker') {
            var id = '#'+ $(this).attr('id') +'-datepicker';
            $(id).dpDisplay();
          }
          $(this).val('_none');
          return false;
        });
	    $('.date-picker')
	      .datePicker({
	        createButton: false,
          displayClose: true,
          closeOnSelect: false,
          selectMultiple: true
        })
	    	.bind('dateSelected', function (e, selectedDate, $td, state) {
          var selectedDates = $(this).dpGetSelected();
          var result = new Array();

          for (var i in selectedDates) {
            result.push(selectedDates[i].asString('yyyy-mm-dd'));
          }

          $('input[name=when_date]').val(result.join(';'));
        });
      }
  };
})(jQuery);