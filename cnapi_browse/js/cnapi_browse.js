(function ($) {

$.dpText = {
  TEXT_PREV_YEAR		:	'Vorig jaar',
  TEXT_PREV_MONTH		:	'Vorige maand',
  TEXT_NEXT_YEAR		:	'Volgend jaar',
  TEXT_NEXT_MONTH		:	'Volgende maand',
  TEXT_CLOSE			  :	'Sluiten',
  TEXT_CHOOSE_DATE	:	'Kies een datum',
  HEADER_FORMAT		  :	'mmmm yyyy'
};

Drupal.behaviors.cnapiBrowseAutoSubmitForm = {
  attach: function (context, settings) {
    $('.autosubmit').change(function() {
      if ($(this).val() != '_none') {
        $(this).parents('form').submit();
        $(':input', $(this).parents('form')).attr('disabled', true);
      }
    });
  }
};

Drupal.behaviors.cnapiBrowseDatePicker = {
  attach: function (context, settings) {

    // hide the date field
    $('.form-item:has(.date-for-datepicker)', context).hide();

    // hide the date field for fields and add the datepicker
    $('.form-item:has(.date-for-datepicker)', context).after('<div class="datepicker"></div>');

    // add extra option to select field to display datepicker
    $('.has-datepicker', context).append($("<option></option>").val("_datepicker").text('Kies zelf data'));

    // bind onchange event to show the datepicker when the "pick a date" option has been chosen
    $('.has-datepicker', context).bind('change', function () {
      if ($(this).val() == '_datepicker') {
        $('.datepicker', $(this).parents('form')).dpDisplay();

        // we need to reset the selected option so we won't submit invalid options to Drupal
        $(this).val('_none');
      }

      return false;
    });

    // create the datepicker and bind the "dateSelected" event
    $('.datepicker', context)
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
    $('.date-for-datepicker', context).each(function () {
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