<?php

function cnapi_browse_advanced_search_form() {
  $form = array();
  
  $options = array();
  
  $report = cnapi_browse_get_event_report();

  if (isset($report['headings'])) {
    $report_headings = $report['headings'];
    
    if (cnapi_output_type_has_heading_groups()) {
      $report_headings = array();
    
      foreach ($report['headings'] as $report_item) {
        if (isset($report_item['children'])) {
          foreach ($report_item['children'] as $id => $child_report_item) {
            $report_headings[$id] = $child_report_item;
          }
        }
      }
    }
    
    uasort($report_headings, 'cnapi_report_sort_total');
    
    foreach ($report_headings as $id => $heading) {
      $options[$id] = $heading['name'];
    }
  }
  
  $form['what'] = array(
    '#type' => 'fieldset',
    '#title' => t('What'),
    '#collapsible' => FALSE,
  );

  $form['what']['query'] = array(
    '#type' => 'textfield',
    '#title' => t('Keywords'),
    '#size' => 20,
  );

  $form['what']['headings'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Category'),
    '#options' => $options,
  );
  
  if (cnapi_output_type_has_heading_groups()) {
    $form['what']['headings']['#theme'] = 'cnapi_browse_headings_element';
  }
  
  $form['where'] = array(
    '#type' => 'fieldset',
    '#title' => t('Where'),
    '#collapsible' => FALSE,
  );

  $form['where']['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#autocomplete_path' => 'cnapi/autocomplete/location',
    '#size' => 20,
  );
  
  $form['where']['radius'] = array(
    '#type' => 'textfield',
    '#prefix' => '+',
    '#suffix' => 'km',
    '#size' => 2,
  );
  
  $provinces = cnapi_get_provinces();
  $options = cnapi_get_select_options($provinces, 'lid', 'name', t('Choose a province'));

  $form['where']['province'] = array(
    '#type' => 'select',
    '#title' => t('Province'),
    '#options' => $options,
  );
  
  $regions = cnapi_get_regions_tourist();
  $options = cnapi_get_select_options($regions, 'lid', 'name', t('Choose a region'));

  $form['where']['region'] = array(
    '#type' => 'select',
    '#title' => t('Region'),
    '#options' => $options,
  );
  
  $form['when'] = array(
    '#type' => 'fieldset',
    '#title' => t('When'),
    '#collapsible' => FALSE,
  );
    
  $form['when']['when_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Date'),
    '#default_value' => '',
    '#description' => t('Format is yyyy/mm/dd'),
    '#size' => 10,
    '#attributes' => array('class' => array('date-for-datepicker')),
  );
  
  $valid_types = array('today', 'tomorrow', 'thisweek', 'thisweekend', 'nextweekend', 'next30days');
  $datetypes = array_intersect_key(cnapi_get_datetypes(), array_flip($valid_types));

  $options = array(CNAPI_OPTION_NONE => t('Select a date from this list')) + $datetypes;

  $form['when']['when'] = array(
    '#type' => 'select',
    '#title' => t('Period'),
    '#options' => $options,
    '#default_value' => -1,
  );
  
  $form['#attached']['library'][] = array('cnapi_browse', 'jquery.datePicker');
  $form['#attached']['js'][] = drupal_get_path('module', 'cnapi_browse') . '/js/cnapi_browse.js';
  
  $form['extra'] = array(
    '#type' => 'fieldset',
    '#title' => t('Extra'),
    '#collapsible' => FALSE,
  );
	
  $options = array(
    'isfree:true' => t('Free offer'),
    'type:!0.50.6.0.0' => t('Exclude movies'),
    'age:12' => t('Exclude events for children'),
    'type:!0.3.1.0.0' => t('Exclude courses/workshops'),
    'permanent:false' => t('Exclude permanent offer'),
  );

  $form['extra']['extra'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#multiple' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  return $form;
}

function cnapi_browse_advanced_search_form_submit($form, &$form_state) {
  // The search form relies on control of the redirect destination for its
  // functionality, so we override any static destination set in the request,
  // for example by drupal_access_denied() or drupal_not_found()
  // (see http://drupal.org/node/292565).
  if (isset($_GET['destination'])) {
    unset($_GET['destination']);
  }

  // initialising the request object with the event context
  $request = array('context' => 'event');
  
  cnapi_browse_process_form_values($request, $form_state['values']);

  cnapi_ui_goto($request);
}