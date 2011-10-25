<?php

function cnapi_browse_get_event_report($query = array(), $skip_heading_groups = FALSE, $sort_types = array(), $sort_heading_groups = TRUE, $parse_geo = TRUE, $fetch_movie_total = TRUE) {
  cnapi_ui_apply_context_query($query, 'event');

  $report = cnapi_get_report('event', $query, $parse_geo);

  // movies as productions
  $movies_context = cnapi_ui_contexts('id', 'movie');

  if ($movies_context) {
    $movies_query = $query;
    cnapi_ui_remove_context_query($movies_query, 'event');
    unset($movies_query['heading']);
    cnapi_ui_apply_context_query($movies_query, 'movie');

    $movie_item = NULL;
    if (isset($report['headings'][64])) {
      $movie_item = &$report['headings'][64];
    }
    elseif (isset($report['headings'][176]['children'][64])) {
      $movie_item = &$report['headings'][176]['children'][64];
    }

    if ($movie_item) {
      if ($fetch_movie_total) {
        $movies = cnapi_get_productions($movies_query);
        $movie_item['total'] = $movies['total'];
      }
      $movie_item['link']['type'] = 'production';
      cnapi_ui_remove_context_query($movie_item['link']['query'], 'event');
      unset($movie_item['link']['query']['heading']);
      $movie_item['link']['context'] = 'movie';
    }
  }

  // children movies as productions
  $movies_children_context = cnapi_ui_contexts('id', 'movie_children');

  if ($movies_children_context) {
    $movies_children_query = $query;
    cnapi_ui_remove_context_query($movies_children_query, 'event');
    unset($movies_children_query['heading']);
    cnapi_ui_apply_context_query($movies_children_query, 'movie_children');

    $movies_children_item = NULL;
    if (isset($report['headings'][98])) {
      $movies_children_item = &$report['headings'][98];
    }
    elseif (isset($report['headings'][179]['children'][98])) {
      $movies_children_item = &$report['headings'][179]['children'][98];
    }

    if ($movies_children_item) {
      if ($fetch_movie_total) {
        $movies_children = cnapi_get_productions($movies_children_query);
        $movies_children_item['total'] = $movies_children['total'];
      }
      $movies_children_item['link']['type'] = 'production';
      cnapi_ui_remove_context_query($movies_children_item['link']['query'], 'event');
      unset($movies_children_item['link']['query']['heading']);
      $movies_children_item['link']['context'] = 'movie_children';
    }
  }

  if (cnapi_output_type_has_heading_groups() && isset($report['headings']) && $skip_heading_groups) {
    $report_headings = array();

    foreach ($report['headings'] as $report_item) {
      if (isset($report_item['children'])) {
        foreach ($report_item['children'] as $id => $child_report_item) {
          $report_headings[$id] = $child_report_item;
        }
      }
    }

    $report['headings'] = $report_headings;
  }

  if (!empty($sort_types)) {
    foreach ($sort_types as $type) {
      if (isset($report['headings']) && $type == 'headings' && cnapi_output_type_has_heading_groups() && !$sort_heading_groups && !$skip_heading_groups) {
        foreach ($report['headings'] as $id => $subreport) {
          if (!empty($subreport['children'])) {
            cnapi_browse_sort_subreport($report['headings'][$id]['children']);
          }
        }
      }
      else {
        if (isset($report[$type])) {
          cnapi_browse_sort_subreport($report[$type]);
        }
      }
    }
  }

  return $report;
}

function cnapi_browse_sort_subreport(&$report) {
  uasort($report, 'cnapi_report_sort_total');

  foreach ($report as $id => $subreport) {
    if (!empty($subreport['children'])) {
      cnapi_browse_sort_subreport($report[$id]['children']);
    }
  }
}

function cnapi_browse_process_form_values(&$request, $values) {
  // what
  if (isset($values['query'])) {
    $query = $values['query'];
    if (!empty($query)) {
      $request['query']['query'] = $query;
    }
  }
  if (isset($values['headings']) && is_array($values['headings'])) {
    $headings = array_filter($values['headings']);

    if (!empty($headings)) {
      $request['query']['heading'] = implode(';', $headings);
    }
  }
  if (isset($values['cdbid']) && $values['cdbid'] != CNAPI_OPTION_NONE) {
    $request['query']['cdbid'] = $values['cdbid'];
    unset($request['query']['heading']);
    unset($request['query']['query']);
  }

  // where
  $filter_is_set = FALSE;

  $city = isset($values['city']) ? $values['city'] : NULL;
  if (!empty($city)) {
    $city_parsed = cnapi_location_parse($city);

    if ($city_parsed) {
      $type = $city_parsed['type'];
      $value = $city_parsed['value'];

      $request['query'][$type] = $value;
    }
    else {
      $request['query']['city'] = $city;
    }

    // radius
    $radius = isset($values['radius']) ? $values['radius'] : NULL;
    if (!empty($radius) && is_numeric($radius)) {
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

  // region (only if no city or province was chosen)
  $location = isset($values['location']) ? $values['location'] : NULL;
  if (!$filter_is_set && !empty($location) && $location != CNAPI_OPTION_NONE) {
    $request['query']['location'] = $location;
  }

  // province (only if no city or location was chosen)
  $province = isset($values['province']) ? $values['province'] : NULL;
  if (!$filter_is_set && !empty($province) && $province != CNAPI_OPTION_NONE) {
    $request['query']['regio'] = $province;
  }

  // region (only if no city or province was chosen)
  $region = isset($values['region']) ? $values['region'] : NULL;
  if (!$filter_is_set && !empty($region) && $region != CNAPI_OPTION_NONE) {
    $request['query']['regio'] = $region;
  }

  // when date
  $when_date = isset($values['when_date']) ? $values['when_date'] : NULL;
  if (!empty($when_date)) {
    $request['query']['date'] = $when_date;
  }

  // when
  $when = isset($values['when']) ? $values['when'] : NULL;
  if (!empty($when) && $when != CNAPI_OPTION_NONE && empty($when_date)) {
    $request['query']['datetype'] = $when;
  }

  // extra
  $extra = isset($values['extra']) && is_array($values['extra']) ? array_filter($values['extra']) : array();
  if (!empty($extra)) {
    foreach ($extra as $option) {
      list($key, $value) = explode(':', $option);
      $request['query'][$key] = $value;
    }
  }

  drupal_alter('cnapi_browse_process_form_values', $request, $values);
}