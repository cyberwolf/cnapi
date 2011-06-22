<?php

function cnapi_browse_block_navigation() {
  $report = cnapi_browse_get_event_report();

  $items = array();
  foreach ($report['headings'] as $report_item) {
    $request = cnapi_url_p2dp($report_item['link'], 'event');
    $items[] = cnapi_url_dp2dul($report_item['name'], $request);
  }
  
  $block['subject'] = t('Navigation');
  $block['content'] = theme('item_list', array('items' => $items));

  return $block;
}

function cnapi_browse_block_search() {
  $block['subject'] = t('Search');
  $block['content'] = drupal_get_form('cnapi_browse_search_form');

  return $block;
}

function cnapi_browse_search_form() {
  $form = array();
  
  $form['what'] = array(
    '#type' => 'textfield',
    '#title' => t('What'),
  );
  
  $form['where'] = array(
    '#type' => 'textfield',
    '#title' => t('Where'),
    '#autocomplete_path' => 'cnapi/autocomplete/location',
  );
  
  $options = array(-1 => t('Select a date from this list')) + cnapi_get_datetypes();
  
  $form['when'] = array(
    '#type' => 'select',
    '#title' => t('When'),
    '#options' => $options,
    '#default_value' => -1,
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  return $form;
}

function cnapi_browse_search_form_submit($form, &$form_state) {
  // The search form relies on control of the redirect destination for its
  // functionality, so we override any static destination set in the request,
  // for example by drupal_access_denied() or drupal_not_found()
  // (see http://drupal.org/node/292565).
  if (isset($_GET['destination'])) {
    unset($_GET['destination']);
  }
  
  // initialising the request object with the event context
  $request = array('context' => 'event');
  
  // what
  $what = $form_state['values']['what'];
  if (!empty($what)) {
    $request['query']['query'] = $what;
  }
  
  // where
  $where = $form_state['values']['where'];
  if (!empty($where)) {
    $where_parsed = cnapi_location_parse($where);

    if ($where_parsed) {
      $type = $where_parsed['type'];
      $value = $where_parsed['value'];
      
      $request['query'][$type] = $value;
    }
    else {
      $request['query']['city'] = $where;
    }
  }
  
  // when
  $when = $form_state['values']['when'];
  if ($when != -1) {
    $request['query']['datetype'] = $when;
  }
  
  cnapi_ui_goto($request);
}