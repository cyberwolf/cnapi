<?php

function cnapi_browse_block_navigation() {
  $report = cnapi_browse_get_event_report();

  if (!isset($report['headings'])) {
    return;
  }

  $report_headings = $report['headings'];

  if (!cnapi_output_type_has_heading_groups()) {
    uasort($report_headings, 'cnapi_report_sort_total');
  }
  else {
    foreach ($report_headings as $id => $report_item) {
      if (isset($report_headings[$id]['children'])) {
        uasort($report_headings[$id]['children'], 'cnapi_report_sort_total');
      }
    }
  }

  $items = array();
  foreach ($report_headings as $report_item) {
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

  $block['subject'] = t('Browse all categories');
  $block['content'] = theme('cnapi_browse_navigation', array('items' => $items));

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
    '#size' => 20,
  );

  $form['where'] = array(
    '#type' => 'textfield',
    '#title' => t('Where'),
    '#autocomplete_path' => 'cnapi/autocomplete/location',
    '#size' => 20,
  );

  $valid_types = array('today', 'tomorrow', 'thisweek', 'thisweekend', 'nextweekend', 'next30days');
  $datetypes = array_intersect_key(cnapi_get_datetypes(), array_flip($valid_types));

  $options = array(CNAPI_OPTION_NONE => t('Select a date from this list')) + $datetypes;

  $form['when'] = array(
    '#type' => 'select',
    '#title' => t('When'),
    '#options' => $options,
    '#default_value' => -1,
    '#attributes' => array('class' => array('has-datepicker')),
  );
  
  $form['when_date'] = array(
    '#type' => 'hidden',
    '#default_value' => '',
  );
  
  $form['#attached']['library'][] = array('cnapi_browse', 'jquery.datePicker');
  $form['#attached']['js'][] = drupal_get_path('module', 'cnapi_browse') . '/js/cnapi_browse.js';

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
  if ($when != CNAPI_OPTION_NONE) {
    $request['query']['datetype'] = $when;
  }
  
  // when date
  $when_date = $form_state['values']['when_date'];
  if (!empty($when_date)) {
    $request['query']['date'] = $when_date;
  }

  cnapi_ui_goto($request);
}

function cnapi_browse_block_filters() {
  $request = cnapi_ui_get_active_request();

  if (!$request) {
    return;
  }

  $context = cnapi_ui_contexts('id', $request['context']);

  if (!$context) {
    return;
  }
  elseif (!in_array($context['type'], array('event', 'production'))) {
    return;
  }

  $report_query = $request['query'];
  cnapi_ui_apply_context_query($report_query, $request['context']);

  if ($context['type'] == 'event') {
    $report = cnapi_browse_get_event_report($report_query);
  }
  elseif ($context['type'] == 'production') {
    $report = cnapi_get_report('production', $report_query);
  }

  $output = '';

  if ($context['type'] == 'event') {
    $output .= '<h2>Wat</h2>';
    $output .= cnapi_browse_filter_what_event($report);
  }

  $output .= '<h2>Where</h2>';
  $output .= cnapi_browse_filter_where($report, $context['type']);

  $output .= '<h2>Wanneer</h2>';
  $output .= cnapi_browse_filter_when($report);

  $block['subject'] = t('Filter search results');
  $block['content'] = $output;

  return $block;
}

function cnapi_browse_filter_what_event($report) {
  if (!isset($report['headings'])) {
    return;
  }

  $request = cnapi_ui_get_active_request();

  $headings = cnapi_get_headings_tree(NULL, 'flat');

  $request_headings = isset($request['query']['heading']) ? explode(';', $request['query']['heading']) : array();

  $active_headings = array();

  $the_depth = cnapi_output_type_has_heading_groups() ? 2 : 1;

  foreach ($request_headings as $active_heading) {
    if (!cnapi_output_type_has_heading_groups()) {
      $active_headings[] = $active_heading;
    }
    elseif ($headings[$active_heading]['depth'] != 0) {
      $active_headings[] = $active_heading;
    }

    if ($headings[$active_heading]['depth'] == $the_depth) {
      $active_headings[] = $headings[$active_heading]['pid'];
    }
  }

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
  foreach ($report_headings as $id => $report_item) {
    if (isset($report_headings[$id]['children'])) {
      uasort($report_headings[$id]['children'], 'cnapi_report_sort_total');
    }
  }

  $filter_options = array();

  foreach ($report_headings as $report_item) {
    if (cnapi_output_type_has_heading_groups() && !empty($active_headings) && !in_array($report_item['id'], $active_headings)) {
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

  // @todo add option "all headings"

  return theme('cnapi_browse_filter_options', array('options' => $filter_options));
}

function cnapi_browse_filter_where($report, $type) {
  $request = cnapi_ui_get_active_request();

  $report_geo = isset($report['geo']) ? $report['geo'] : NULL;

  $request_remove = $request;
  unset($request_remove['query']['regio']);
  unset($request_remove['query']['zip']);
  unset($request_remove['query']['city']);
  unset($request_remove['query']['cityid']);
  unset($request_remove['query']['location']);

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

      $name = $municipalities[$region]['name'];
      if ($report_geo) {
        $total = $report_geo[$province]['children'][$pid]['children'][$region_tourist]['total'];
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
  elseif (isset($request['query']['zip'])) {
    $name = $request['query']['zip'];

    $id = $zip;

    if ($report_geo) {
      $total = NULL;
    }
  }
  elseif (isset($request['query']['city'])) {
    $name = $request['query']['city'];

    $id = $request['query']['city'];

    if ($report_geo) {
      $total = NULL;
    }
  }
  elseif (isset($request['query']['cityid'])) {
    $cityid = $request['query']['cityid'];

    $id = cityid;

    $cities = cnapi_get_cities($request['query']['cityid']);
    if (isset($cities[$cityid])) {
      $name = $cities[$cityid]['name'];
    }

    $region_municipality = db_query("SELECT pid FROM {cnapi_location_hierarchy} WHERE lid = :lid AND pid IN (SELECT lid FROM {cnapi_location} WHERE did = :did)", array(':lid' => $cityid, ':did' => CNAPI_LOCATIONS_DIMENSION_MUNICIPALITY))->fetchField();
    $region_tourist = db_query("SELECT pid FROM {cnapi_location_hierarchy} WHERE lid = :lid AND pid IN (SELECT lid FROM {cnapi_location} WHERE did = :did)", array(':lid' => $cityid, ':did' => CNAPI_LOCATIONS_DIMENSION_TOURIST))->fetchField();
    $province = db_query("SELECT pid FROM {cnapi_location_hierarchy} WHERE lid = :lid AND pid IN (SELECT lid FROM {cnapi_location} WHERE did = :did)", array(':lid' => $region_tourist, ':did' => CNAPI_LOCATIONS_DIMENSION_PROVINCE))->fetchField();

    // @todo dimension depends on key?

    if ($report_geo) {
      $total = $report_geo[$province]['children'][$region_tourist]['children'][$region_municipality]['children'][$cityid]['total'];
    }
  }

  if (isset($name)) {
    $filter_options = array();

    $filter_option = array(
      'id' => NULL,
      'name' => t('In Flanders'),
      'url' => $request_remove,
      'total' => 0, // @todo get total
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

  $form = drupal_get_form('cnapi_browse_filter_where_form', $report, $type);

  $output = '';
  if (isset($filter_options)) {
    $output .= theme('cnapi_browse_filter_options', array('options' => $filter_options));
  }
  $output .= drupal_render($form);

  return $output;
}

function cnapi_browse_filter_where_form($form, &$form_state, $report, $type) {
  $form = array();

  $provinces = cnapi_get_provinces();
  $options = cnapi_get_select_options($provinces, 'lid', 'name', t('Choose a province'));

  $form['province'] = array(
    '#type' => 'select',
    '#title' => t('Province'),
    '#options' => $options,
  );

  if ($type == 'event') {
    $form['city'] = array(
      '#type' => 'textfield',
      '#title' => t('City'),
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

    $options = array(CNAPI_OPTION_NONE => t('Select a location')) + $options;

    $form['location'] = array(
      '#type' => 'select',
      '#title' => t('Location'),
      '#options' => $options,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );

  return $form;
}

function cnapi_browse_filter_where_form_submit($form, &$form_state) {
  $request = cnapi_ui_get_active_request();

  // resetting all "where" filters
  unset($request['query']['city']);
  unset($request['query']['cityid']);
  unset($request['query']['zip']);
  unset($request['query']['regio']);
  unset($request['query']['location']);

  $filter_is_set = FALSE;

  // city and radius
  $city = isset($form_state['values']['city']) ? $form_state['values']['city'] : NULL;
  if (!empty($city)) {
    // city
    $city_parsed = cnapi_location_parse($city);

    if ($city_parsed) {
      $type = $city_parsed['type'];
      $value = $city_parsed['value'];

      $request['query'][$type] = $value;
    }
    else {
      $request['query']['city'] = $where;
    }

    // radius
    $radius = $form_state['values']['radius'];
    if (!empty($radius) && !empty($city) && is_numeric($radius)) {
      if (!$city_parsed) {
        $request['query']['city'] . '!' . $radius . 'km';
      }
      elseif (in_array($city_parsed['type'], array('city', 'cityid', 'zip'))) {
        $type = $city_parsed['type'];
        $request['query'][$type] .= '!' . $radius . 'km';
      }
    }

    $filter_is_set = TRUE;
  }

  // location (only if no city was chosen)
  if (!$filter_is_set && isset($form_state['values']['location'])) {
    $location = $form_state['values']['location'];
    if ($location != CNAPI_OPTION_NONE) {
      $request['query']['location'] = $location;

      $filter_is_set = TRUE;
    }
  }

  // province (only if no city or location was chosen)
  $province = $form_state['values']['province'];
  if (!$filter_is_set && $province != CNAPI_OPTION_NONE) {
    $request['query']['regio'] = $province;
  }

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
      unset($request_remove['query']['datetype']);
      $filter_option['url_remove'] = $request_remove;
      $filter_option['active'] = TRUE;
    }

    // add the option
    $filter_options[] = $filter_option;
  }

  return theme('cnapi_browse_filter_options', array('options' => $filter_options));
}