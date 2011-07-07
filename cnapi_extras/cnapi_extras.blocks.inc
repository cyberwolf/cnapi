<?php

/**
 * The wider radius block provides a block on cnapi search results pages that have no results.
 * If no results were found and the request contains a filter on city or cityid, we will iteratively search in a wider and wider radius from that city for possible results.
 */
function cnapi_extras_block_wider_radius_search() {
  $request = cnapi_ui_get_active_request();
  
  $context = cnapi_ui_contexts('id', $request['context']);
  
  // We only act upon cnapi requests.
  if (!$request) {
    return;
  }

  // Convert the $request to API format.
  $request_p = cnapi_url_dp2p($request);

  // Do the request.
  $objects = cnapi_get($request_p);
  
  // If we didn't receive a 'total' (something went wrong) or we have results, this block is not usefull, so return.
  if (!isset($objects['total']) || $objects['total'] > 0) {
    return;
  }

  // Check the request for a city or cityid query part.
  $type = NULL;
  $city = NULL;
  
  if (isset($request_p['query']['city'])) {
    $type = 'city';
    $city = $request_p['query']['city'];
  }
  elseif (isset($request_p['query']['cityid'])) {
    $type = 'cityid';
    $city = $request_p['query']['cityid'];
  }
  
  // If we're not on a city results page, we can't search within a wider radius, so this block is useless.
  if (!$city) {
    return;
  }
  
  // Parse the city and radius part from the city query part.
  $city_parts = explode('!', $city);
  $city = $city_parts[0];
  $radius = isset($city_parts[1]) ? intval(str_replace('km', '', $city_parts[1])) : 0;

  // Inititate some variables to keep track of the found radius, data, ...
  $found_radius = FALSE;
  $found_request = NULL;
  $found_data = NULL;
  $found_total = NULL;

  // $search_radia will hold all radia we will do a search for.
  $search_radia = array(5, 10, 20, 30);

  // Loop over all the search radia.
  foreach ($search_radia as $search_radius) {
    // If a radius was specified in the request and that radius is smaller then the current one in the loop, we skip this one.
    if ($radius > 0 && $radius <= $search_radius) {
      continue;
    }
    
    // Construct the query for the wider radius query...
    $request_p['query'][$type] = $city . '!' . $search_radius . 'km';

    // ... and do it.
    $result = cnapi_get($request_p);

    // If we have found results, save it in the $found... variables.
    if (isset($result['total']) && isset($result['data']) && $result['total'] > 0) {
      $found_radius = $search_radius;
      $found_request = $request_p;
      $found_data = $result['data'];
      $found_total = $result['total'];

      // And stop iterating.
      break;
    }
  }
  
  if (!$found_radius) {
    // Construct the query for searching over the whole of flanders.
    unset($request_p['query'][$type]);

    // ... and do it.
    $result = cnapi_get($request_p);

    // If we have found results, save it in the $found... variables.
    if (isset($result['total']) && isset($result['data']) && $result['total'] > 0) {
      $found_radius = -1;
      $found_request = $request_p;
      $found_data = $result['data'];
      $found_total = $result['total'];
    }
  }
  
  if ($found_total) {
    $block['subject'] = t('Wider radius search');
    $block['content'] = array(
      '#theme' => 'cnapi_extras_block_wider_radius_search',
      '#request_type' => $context['type'],
      '#city' => $city,
      '#city_type' => $type,
      '#radius' => $found_radius,
      '#request' => cnapi_url_p2dp($found_request, $request['context']),
      '#data' => $found_data,
      '#total' => $found_total,
    );
  }
  
  return $block;
}