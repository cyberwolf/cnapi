<?php

function cnapi_extras_block_movie_results() {
  $request = cnapi_ui_get_active_request();
  
  if (isset($request['context'])) {
    $context = cnapi_ui_contexts('id', $request['context']);
    if ($context['type'] != 'event') {
      return;
    }
  }

  if (isset($request['query']['heading'])) {
    return;
  }

  if (isset($request['query']['facilities']) && in_array($request['query']['facilities'], array('3.21.0.0.0', '3.12.0.0.0', '3.13.0.0.0', '3.12.0.0.0', '3.17.0.0.0', '3.14.0.0.0'))) {
    return;
  }

  if (isset($request['query']['isfree'])) {
    return;
  }

  if (isset($request['query']['type'])) {
    return;
  }

	if (isset($request['query']['thema']) && $request['query']['thema'] != '1.7.0.0.0') {
    return;
  }

  if (isset($request['query']['page']) && $request['query']['page'] > 0) {
    return;
  }
  
  $movie_context = cnapi_ui_contexts('id', 'movie');

  $query = $request['query'];
  
  foreach ($movie_context['query'] as $key => $value) {
    $query[$key] = $value;
  }
  
  $query['pagelength'] = 5;
  
  $productions = cnapi_get_productions($query);
  
  if (empty($productions['data'])) {
    return;
  }

  unset($query['pagelength']);
  $more_request = array('context' => 'movie', 'query' => $query);
  
  $block['subject'] = 'Meest vertoonde films voor uw zoekopdracht';
  $block['content'] = array(
    '#theme' => 'cnapi_extras_block_movie_results',
    '#productions' => $productions['data'], 
    '#form' => drupal_render(drupal_get_form('cnapi_extras_movie_search_form')),
    '#total' => $productions['total'],
    '#more_request' => $more_request,
  );
  
  return $block;
}

/**
 * The wider radius block provides a block on cnapi search results pages that have no results.
 * If no results were found and the request contains a filter on city or cityid, we will iteratively search in a wider and wider radius from that city for possible results.
 * If we still can't find results within the maximum radius, we expand to the whole of Flanders.
 */
function cnapi_extras_block_wider_radius_search() {
  // Get the current request.
  $request = cnapi_ui_get_active_request();

  // We only act upon cnapi requests.
  if (!$request || !isset($request['context'])) {
    return;
  }

  // Get full info on the context of the current request.
  $context = cnapi_ui_contexts('id', $request['context']);

  // Convert the $request to API format.
  $request_p = cnapi_url_dp2p($request);

  // Do the request.
  $objects = cnapi_get($request_p);

  // If we didn't receive a 'total' (something went wrong) or we have results, this block is useless, so return.
  if (!isset($objects['total']) || $objects['total'] > 0) {
    return;
  }

  // Check the request for a city or cityid query part (type and value). If we don't have one of those, this block is useless.
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

  // If we couldn't find some results for one of the valid radia, we search for the same query in the whole of Flanders (no city limitation).
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

  // Return the themed block with all info in case we found something...
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

    return $block;
  }
  
  // ... else, do nothing.
}