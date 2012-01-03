<?php

function cnapi_browse_block_is_visible_active_filters() {
  $request = cnapi_ui_get_active_request();
  
  if (!isset($request['context'])) {
    return FALSE;
  }
  
  return TRUE;
}

function cnapi_browse_block_cache_id_active_filters() {
  $request = cnapi_ui_get_active_request();
  
  $request = cnapi_url_dp2p($request);
  
  unset($request['query']['page']);
  unset($request['query']['pagelength']);
  unset($request['query']['sort']);
  
  return array(cnapi_request_hash($request));
}

function cnapi_browse_block_view_active_filters() {
  $request = cnapi_ui_get_active_request();
  
  $filter_options = array();
  
  $filters = cnapi_ui_get_active_request_filters();

  if (!$filters) {
    return;
  }
  
  foreach ($filters as $key => $key_filters) {
    foreach ($key_filters as $filter) {
      $filter_options[] = array(
        'id' => $filter['value'],
        'name' => $filter['description'],
        'url' => $request,
        'active' => TRUE,
        'url_remove' => $filter['url_remove'],
      );
    }
  }

  $block['subject'] = 'Actieve filters';
  $block['content'] = theme('cnapi_browse_filter_options', array('options' => $filter_options));
  
  return $block;
}

function cnapi_browse_block_view_navigation() {
  $report = cnapi_browse_get_event_report(array(), FALSE, array('headings'), FALSE, FALSE);

  if (!isset($report['headings']) || empty($report['headings'])) {
    return;
  }

  $items = array();
  foreach ($report['headings'] as $report_item) {
    $link_context = isset($report_item['link']['context']) ? $report_item['link']['context'] : 'event';

    $item = array(
      'id' => $report_item['id'],
      'name' => $report_item['name'],
      'url' => cnapi_url_p2dp($report_item['link'], $link_context),
      'total' => $report_item['total'],
      'is_heading_group' => cnapi_output_type_has_heading_groups(),
    );

    if (cnapi_output_type_has_heading_groups() && isset($report_item['children'])) {
      $child_items = array();

      foreach ($report_item['children'] as $child_report_item) {
        $link_context = isset($child_report_item['link']['context']) ? $child_report_item['link']['context'] : 'event';
        $child_items[] = array(
          'id' => $child_report_item['id'],
          'name' => $child_report_item['name'],
          'url' => cnapi_url_p2dp($child_report_item['link'], $link_context),
          'total' => $child_report_item['total'],
          'is_heading_group' => FALSE,
        );
      }

      if (!empty($child_items)) {
        $item['children'] = $child_items;
      }
    }

    $items[] = $item;
  }

  $block['subject'] = 'Bekijk alle rubrieken';
  $block['content'] = theme('cnapi_browse_navigation', array('items' => $items));

  return $block;
}

function cnapi_browse_block_view_search() {
  $block['subject'] = 'Zoek';
  $block['content'] = drupal_get_form('cnapi_browse_search_form');

  return $block;
}

function cnapi_browse_search_form() {
  $form = array();

  $form['query'] = array(
    '#type' => 'textfield',
    '#title' => 'Wat',
    '#size' => 20,
  );

  $form['city'] = array(
    '#type' => 'textfield',
    '#title' => 'Waar',
    '#autocomplete_path' => 'cnapi/autocomplete/location',
    '#size' => 20,
  );

  $valid_types = array('today', 'tomorrow', 'thisweek', 'thisweekend', 'nextweekend', 'next30days');
  $datetypes = array_intersect_key(cnapi_get_datetypes(), array_flip($valid_types));

  $options = array(CNAPI_OPTION_NONE => 'Selecteer een datum') + $datetypes;

  $form['when'] = array(
    '#type' => 'select',
    '#title' => 'Wanneer',
    '#options' => $options,
    '#default_value' => -1,
    '#attributes' => array('class' => array('has-datepicker')),
  );

  $form['when_date'] = array(
    '#title' => 'Datum',
    '#type' => 'textfield',
    '#default_value' => '',
    '#description' => 'Formaat yyyy/mm/dd',
    '#size' => 10,
    '#maxlength' => 1024,
    '#attributes' => array('class' => array('date-for-datepicker')),
  );

  $form['#attached']['library'][] = array('cnapi_browse', 'jquery.datePicker');
  $form['#attached']['js'][] = drupal_get_path('module', 'cnapi_browse') . '/js/cnapi_browse.js';

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Zoek',
  );

  $form['advanced_search'] = array(
    '#markup' => '<div id="link-advanced-search">' . l('Uitgebreid zoeken', 'agenda/s') . '</div>',
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

  cnapi_browse_process_form_values($request, $form_state['values']);

  cnapi_ui_goto($request);
}

function cnapi_browse_block_is_visible_filters_what() {
  return cnapi_browse_block_is_visible_filters();
}

function cnapi_browse_block_is_visible_filters_where() {
  return cnapi_browse_block_is_visible_filters();
}

function cnapi_browse_block_is_visible_filters_when() {
  return cnapi_browse_block_is_visible_filters();
}

function cnapi_browse_block_is_visible_filters() {
  $request = cnapi_ui_get_active_request();

  if (!isset($request['context'])) {
    return FALSE;
  }

  $context = cnapi_ui_contexts('id', $request['context']);

  if (!$context) {
    return FALSE;
  }
  elseif (!in_array($context['type'], array('event', 'production'))) {
    return FALSE;
  }
  
  return TRUE;
}

function cnapi_browse_block_cache_id_filters_what() {
  return cnapi_browse_block_cache_id_filters();
}

function cnapi_browse_block_cache_id_filters_when() {
  return cnapi_browse_block_cache_id_filters();
}

function cnapi_browse_block_cache_id_filters() {
  $request = cnapi_ui_get_active_request();
  
  $request = cnapi_url_dp2p($request);
  
  unset($request['query']['page']);
  unset($request['query']['pagelength']);
  unset($request['query']['sort']);
  
  return array(cnapi_request_hash($request));
}

function cnapi_browse_block_view_filters_what() {
  return cnapi_browse_block_view_filters('what', 'Wat');
}

function cnapi_browse_block_view_filters_where() {
  return cnapi_browse_block_view_filters('where', 'Waar');
}

function cnapi_browse_block_view_filters_when() {
  return cnapi_browse_block_view_filters('when', 'Wanneer');
}

function cnapi_browse_block_view_filters($type, $title) {
  $request = cnapi_ui_get_active_request();
  
  $context = cnapi_ui_contexts('id', $request['context']);

  $request_p = cnapi_url_dp2p($request);
  
  $result = cnapi_get($request_p);
  if (!isset($result['total']) || $result['total'] == 0) {
    return;
  }

  $report_query = $request_p['query'];

  cnapi_ui_apply_context_query($report_query, $request['context']);

  unset($report_query['page']);

  if ($context['type'] == 'event') {
    $report = cnapi_browse_get_event_report($report_query, TRUE, array('headings'), TRUE, isset($request['query']['regio']), FALSE);
  }
  elseif ($context['type'] == 'production') {
    $report = cnapi_get_report('production', $report_query, isset($request['query']['regio']));
  }

  if ($type == 'what') {
    if ($context['type'] == 'event') {
      $render = cnapi_browse_filter_what_event($report);
    }
    
    if ($context['type'] == 'production') {
      $render = cnapi_browse_filter_what_production($report);
    }
  }

  if ($type == 'where') {
    $render = cnapi_browse_filter_where($report, $context['type']);
  }

  if ($type == 'when') {
    $render = cnapi_browse_filter_when($report);
  }

  $block['subject'] = $title;
  $block['content'] = drupal_render($render);

  return $block;
}

function cnapi_browse_filter_what_event($report) {
  if (!isset($report['headings']) || empty($report['headings'])) {
    return;
  }

  $request = cnapi_ui_get_active_request();

  $headings = cnapi_get_headings_tree(NULL, 'flat');

  $request_headings = isset($request['query']['heading']) ? explode(';', $request['query']['heading']) : array();

  $active_headings = array();

  $active_heading_groups = array();

  $the_depth = cnapi_output_type_has_heading_groups() ? 2 : 1;

  foreach ($request_headings as $active_heading) {
	  if (!isset($headings[$active_heading])) {
	    continue;
	  }
    if (!cnapi_output_type_has_heading_groups()) {
      $active_headings[] = $active_heading;
    }
    else {
      if ($headings[$active_heading]['depth'] != 0) {
        $active_headings[] = $active_heading;
      }
      else {
        $active_heading_groups[] = $active_heading;
      }
    }

    if ($headings[$active_heading]['depth'] == $the_depth) {
      $active_headings[] = $headings[$active_heading]['pid'];
    }
  }

  $filter_options = array();

  if (isset($request['query']['heading'])) {
    $request_remove = $request;
    unset($request_remove['query']['heading']);

    $request_p = cnapi_url_dp2p($request_remove);

    $result = cnapi_get($request_p);

    $filter_options[] = array(
      'id' => NULL,
      'name' => 'Alle rubrieken',
      'url' => $request_remove,
      'total' => $result['total'],
      'active' => FALSE,
    );

  }

  foreach ($report['headings'] as $id => $report_item) {
    if (cnapi_output_type_has_heading_groups() && !empty($active_headings) && !in_array($report_item['id'], $active_headings)) {
      continue;
    }

    if (!empty($active_heading_groups) && !in_array($headings[$id]['pid'], $active_heading_groups)) {
      continue;
    }

    $filter_context = isset($report_item['link']['context']) ? $report_item['link']['context'] : $request['context'];

    $filter_option = array(
      'id' => $report_item['id'],
      'name' => $report_item['name'],
      'url' => cnapi_url_p2dp($report_item['link'], $filter_context),
      'total' => $report_item['total'],
      'active' => FALSE,
    );

    if (in_array($report_item['id'], $active_headings)) {
      $new_headings = array_diff($request_headings, array($report_item['id'])); // remove element

      foreach ($active_headings as $active_heading) {
        if ($headings[$active_heading]['pid'] == $report_item['id']) {
          $new_headings = array_diff($request_headings, array($active_heading)); // remove element
        }
      }

      $filter_option['url_remove'] = $request;
      unset($filter_option['url_remove']['query']['heading']);
      if (!empty($new_headings)) {
        $filter_option['url_remove']['query']['heading'] = implode(';', $new_headings);
      }

      $filter_option['active'] = TRUE;
    }

    if (in_array($report_item['id'], $active_headings) && isset($report_item['children'])) {
      $child_filter_options = array();

      foreach ($report_item['children'] as $child_report_item) {
        $filter_context = isset($child_report_item['link']['context']) ? $child_report_item['link']['context'] : $request['context'];

        $child_filter_option = array(
          'id' => $child_report_item['id'],
          'name' => $child_report_item['name'],
          'url' => cnapi_url_p2dp($child_report_item['link'], $filter_context),
          'total' => $child_report_item['total'],
          'active' => FALSE,
        );

        if (in_array($child_report_item['id'], $active_headings)) {
          $new_headings = array_diff($new_headings, array($child_report_item['id']));
          $new_headings[] = $headings[$child_report_item['id']]['pid'];

          $child_filter_option['url_remove'] = $request;
          $child_filter_option['url_remove']['query']['heading'] = implode(';', $new_headings);

          $child_filter_option['active'] = TRUE;
        }

        $child_filter_options[] = $child_filter_option;
      }

      if (!empty($child_filter_options)) {
        $filter_option['children'] = $child_filter_options;
      }
    }
    $filter_options[] = $filter_option;
  }

  return array(
    '#theme' => 'cnapi_browse_filter_options',
    '#options' => $filter_options,
  );
}

function cnapi_browse_filter_what_production($report) {
  $render['filter_form'] = drupal_get_form('cnapi_browse_filter_what_production_form', $report);
  
  return $render;
}

function cnapi_browse_filter_what_production_form($form, &$form_state, $report) {
  $form = array();
  
  $request = cnapi_ui_get_active_request();

  $context = cnapi_ui_contexts('id', $request['context']);
  
  $query = $context['query'];
  
  $options = array();
  
  $report = cnapi_get_report('production', $query);
  $productions = $report['production'];
  foreach ($productions as $cdbid => $production) {
    $options[$cdbid] = $production['name'];
  }
  
  asort($options);
  
  $options = array(CNAPI_OPTION_NONE => 'Selecteer een productie') + $options;

  $form['cdbid'] = array(
    '#type' => 'select',
    '#title' => 'Productie',
    '#options' => $options,
    '#default_value' => isset($request['query']['cdbid']) ? $request['query']['cdbid'] : '',
    '#attributes' => array('class' => array('autosubmit')),
  );
  
  $form['query'] = array(
    '#type' => 'textfield',
    '#title' => 'Wat',
    '#description' => 'Zoek op titel/trefwoord',
    '#default_value' => isset($request['query']['query']) ? $request['query']['query'] : '',
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Filter',
  );
  
  return $form;
}

function cnapi_browse_filter_what_production_form_submit($form, &$form_state) {
  // The search form relies on control of the redirect destination for its
  // functionality, so we override any static destination set in the request,
  // for example by drupal_access_denied() or drupal_not_found()
  // (see http://drupal.org/node/292565).
  if (isset($_GET['destination'])) {
    unset($_GET['destination']);
  }

  // initialising the request object with the event context
  $request = cnapi_ui_get_active_request();

  cnapi_browse_process_form_values($request, $form_state['values']);

  cnapi_ui_goto($request);
}

function cnapi_browse_filter_where($report, $type) {
  $request = cnapi_ui_get_active_request();

  unset($request['query']['page']);

  $report_geo = isset($report['geo']) ? $report['geo'] : NULL;

  $request_remove = $request;
  unset($request_remove['query']['regio']);
  unset($request_remove['query']['zip']);
  unset($request_remove['query']['city']);
  unset($request_remove['query']['cityid']);
  unset($request_remove['query']['location']);

  unset($request_remove['query']['page']);

  if (isset($request['query']['regio'])) {
    $region = $request['query']['regio'];

    $id = $region;

    $municipalities = cnapi_get_municipalities();
    $provinces = cnapi_get_provinces();

    if (isset($provinces[$region])) {
      $name = $provinces[$region]['name'];
      if ($report_geo) {
        $total = $report_geo[$region]['total'];
      }
    }
    elseif (isset($municipalities[$region])) {
      $region_tourist = db_query("SELECT pid FROM {cnapi_location_hierarchy} WHERE lid = :lid AND pid IN (SELECT lid FROM {cnapi_location} WHERE did = :did)", array(':lid' => $region, ':did' => CNAPI_LOCATIONS_DIMENSION_TOURIST))->fetchField();
      $province = db_query("SELECT pid FROM {cnapi_location_hierarchy} WHERE lid = :lid AND pid IN (SELECT lid FROM {cnapi_location} WHERE did = :did)", array(':lid' => $region_tourist, ':did' => CNAPI_LOCATIONS_DIMENSION_PROVINCE))->fetchField();

      // @todo dimension depends on key?

      $name = $municipalities[$region]['name'] . ' (Regio)';
      if ($report_geo) {
        $total = $report_geo[$province]['children'][$region_tourist]['children'][$region]['total'];
      }
    }
  }
  elseif (isset($request['query']['location'])) {
    $location = $request['query']['location'];

    $id = $location;

    if (isset($report['location'][$location])) {
      $name = $report['location'][$location]['name'];
      $total = $report['location'][$location]['total'];
    }
  }
  elseif (isset($request['query']['zip']) || isset($request['query']['city']) || isset($request['query']['cityid'])) {
    $parts = array();
    
    if (isset($request['query']['zip'])) {
      $parts = explode('!', $request['query']['zip']);
    
      $id = $parts[0];
      $name = $parts[0];
    
      if ($report_geo) {
        $total = NULL;
      }
    }
    elseif (isset($request['query']['city'])) {
      $parts = explode('!', $request['query']['city']);
    
      $id = $parts[0];
      $name = $parts[0];
    
      if ($report_geo) {
        $total = NULL;
      }
    }
    elseif (isset($request['query']['cityid'])) {
      // take radius search into account!
      $parts = explode('!', $request['query']['cityid']);
    
      $id = $parts[0];
    
      $cities = cnapi_get_cities();
      if (isset($cities[$id])) {
        $name = $cities[$id]['name'];
      }
    
      $region_municipality = db_query("SELECT pid FROM {cnapi_location_hierarchy} WHERE lid = :lid AND pid IN (SELECT lid FROM {cnapi_location} WHERE did = :did)", array(':lid' => $id, ':did' => CNAPI_LOCATIONS_DIMENSION_MUNICIPALITY))->fetchField();
      $region_tourist = db_query("SELECT pid FROM {cnapi_location_hierarchy} WHERE lid = :lid AND pid IN (SELECT lid FROM {cnapi_location} WHERE did = :did)", array(':lid' => $id, ':did' => CNAPI_LOCATIONS_DIMENSION_TOURIST))->fetchField();
      $province = db_query("SELECT pid FROM {cnapi_location_hierarchy} WHERE lid = :lid AND pid IN (SELECT lid FROM {cnapi_location} WHERE did = :did)", array(':lid' => $region_tourist, ':did' => CNAPI_LOCATIONS_DIMENSION_PROVINCE))->fetchField();
    
      // @todo dimension depends on key?
      // We check if the city appears in the report because in case of a radius search, the city might not be in the report.
      if ($report_geo && isset($report_geo[$province]['children'][$region_tourist]['children'][$region_municipality]['children'][$id]['total'])) {
        $total = $report_geo[$province]['children'][$region_tourist]['children'][$region_municipality]['children'][$id]['total'];
      }
    }
    
    if (isset($parts[1])) {
      $name .= ' (+ ' . $parts[1] . ')';
    }
  }

  if (isset($name)) {
    $filter_options = array();

    $request_p = cnapi_url_dp2p($request_remove);

    $result = cnapi_get($request_p);

    $filter_option = array(
      'id' => NULL,
      'name' => 'In Vlaanderen',
      'url' => $request_remove,
      'total' => $result['total'],
      'active' => FALSE,
    );

    $filter_options[] = $filter_option;

    $filter_option = array(
      'id' => $id,
      'name' => $name,
      'url' => $request,
      'active' => TRUE,
      'url_remove' => $request_remove,
    );
    if (isset($total)) {
      $filter_option['total'] = $total;
    }

    $filter_options[] = $filter_option;
  }

  $render = array();

  if (isset($filter_options)) {
    $render['filter_options'] = array(
      '#theme' => 'cnapi_browse_filter_options',
      '#options' => $filter_options,
    );
  }

  $render['filter_form'] = drupal_get_form('cnapi_browse_filter_where_form', $report, $type);

  return $render;
}

function cnapi_browse_filter_where_form($form, &$form_state, $report, $type) {
  $form = array();

  $provinces = cnapi_get_provinces();
  $options = cnapi_get_select_options($provinces, 'lid', 'name', 'Kies een provincie');

  $form['province'] = array(
    '#type' => 'select',
    '#title' => 'Provincie',
    '#options' => $options,
    '#attributes' => array('class' => array('autosubmit')),
  );

  if ($type == 'event') {
    $form['city'] = array(
      '#type' => 'textfield',
      '#title' => 'Stad of gemeente',
      '#autocomplete_path' => 'cnapi/autocomplete/location',
      '#size' => 20,
    );

    $form['radius'] = array(
      '#type' => 'textfield',
      '#prefix' => '+',
      '#suffix' => 'km',
      '#size' => 2,
    );
  }
  elseif ($type == 'production' && isset($report['location']) && !empty($report['location'])) {
    $options = array();
    foreach ($report['location'] as $id => $location) {
      $options[$id] = $location['name'];
    }

    asort($options);

    $options = array(CNAPI_OPTION_NONE => 'Selecteer een locatie') + $options;

    $form['location'] = array(
      '#type' => 'select',
      '#title' => 'Locatie',
      '#options' => $options,
      '#attributes' => array('class' => array('autosubmit')),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Filter',
  );

  return $form;
}

function cnapi_browse_filter_where_form_submit($form, &$form_state) {
  $request = cnapi_ui_get_active_request();

  unset($request['query']['page']);

  // resetting all "where" filters
  unset($request['query']['city']);
  unset($request['query']['cityid']);
  unset($request['query']['zip']);
  unset($request['query']['regio']);
  unset($request['query']['location']);

  cnapi_browse_process_form_values($request, $form_state['values']);

  cnapi_ui_goto($request);
}

function cnapi_browse_filter_when($report) {
  $request = cnapi_ui_get_active_request();

  // if there's no time report, we don't show the time filter
  if (!isset($report['time'])) {
    return '';
  }

  // we're only interested in the "time" section in the report
  $report_time = $report['time'];

  // initiate filter options
  $filter_options = array();

  // filter out unwanted datetypes
  $valid_types = array('today', 'tomorrow', 'thisweek', 'thisweekend', 'nextweekend', 'next30days');
  $report_time = array_intersect_key($report_time, array_flip($valid_types));

  // loop over all report options to build the filter
  foreach ($report_time as $report_item) {
    // resetting custom date
    unset($report_item['link']['query']['date']);

    // initiate the filter option with the link filter, total and active state
    $filter_option = array(
      'id' => $report_item['id'],
      'name' => $report_item['name'],
      'url' => cnapi_url_p2dp($report_item['link'], $request['context']),
      'total' => $report_item['total'],
      'active' => FALSE,
    );

    // if this option is active in the current request filter, mark it as active and provide the "remove filter" url
    if (isset($request['query']['datetype']) && $request['query']['datetype'] == $report_item['id']) {
      $request_remove = $request;

      unset($request_remove['query']['page']);

      unset($request_remove['query']['date']);
      unset($request_remove['query']['datetype']);

      $filter_option['url_remove'] = $request_remove;
      $filter_option['active'] = TRUE;
    }

    // add the option
    $filter_options[] = $filter_option;
  }

  $render = array();

  if (isset($filter_options)) {
    $render['filter_options'] = array(
      '#theme' => 'cnapi_browse_filter_options',
      '#options' => $filter_options,
    );
  }

  $render['filter_form'] = drupal_get_form('cnapi_browse_filter_when_form');

  return $render;
}

function cnapi_browse_filter_when_form($form, &$form_state) {
  $form = array();

  $request = cnapi_ui_get_active_request();

  $form['when_date'] = array(
    '#type' => 'textfield',
    '#title' => 'Datum',
    '#default_value' => isset($request['query']['date']) ? $request['query']['date'] : '',
    '#description' => 'Formaat yyyy/mm/dd',
    '#size' => 10,
    '#attributes' => array('class' => array('date-for-datepicker')),
    '#maxlength' => 1024,
  );

  $form['#attached']['library'][] = array('cnapi_browse', 'jquery.datePicker');
  $form['#attached']['js'][] = drupal_get_path('module', 'cnapi_browse') . '/js/cnapi_browse.js';

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Selecteer',
  );

  return $form;
}

function cnapi_browse_filter_when_form_submit($form, &$form_state) {
  $request = cnapi_ui_get_active_request();

  unset($request['query']['page']);

  // resetting all "when" filters
  unset($request['query']['date']);
  unset($request['query']['datetype']);

  cnapi_browse_process_form_values($request, $form_state['values']);

  cnapi_ui_goto($request);
}