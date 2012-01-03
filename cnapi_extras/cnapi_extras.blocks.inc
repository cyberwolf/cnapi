<?php

function cnapi_extras_block_cache_id_actor_playlist() {
  $request = cnapi_ui_get_active_request();
  
  return array($request['actor']);
}

function cnapi_extras_block_is_visible_actor_playlist() {
  $request = cnapi_ui_get_active_request();

  if (!isset($request['actor'])) {
    return FALSE;
  }
  
  return TRUE;
}

function cnapi_extras_block_view_actor_playlist() {
  $request = cnapi_ui_get_active_request();
  
  $actor = cnapi_get_actor($request['actor']);

  $query = array();
  $query['q'] = $actor['detail']['nl']['title'];
  $query['cityid'] = $actor['contactinfo']['address']['cityid'];
  $query['sort'] = 'date ASC';
  $query['pagelength'] = 100;

  $events = cnapi_get_events($query);

  if (empty($events['data'])) {
    return;
  }
  
  $items = array();
  
  foreach ($events['data'] as $event) {
    $items[] = theme('cnapi_extras_actor_playlist_event', array('event' => $event));
  }

  $block['subject'] = 'Speellijst';
  $block['content'] = array(
    '#theme' => 'item_list',
    '#items' => $items,
    '#attributes' => array('class' => 'actor-playlist')
  );
  
  return $block; 
}

function cnapi_extras_block_cache_id_movie_playlist() {
  $request = cnapi_ui_get_active_request();
  
  return array($request['production']);
}

function cnapi_extras_block_is_visible_movie_playlist() {
  $request = cnapi_ui_get_active_request();

  if (!isset($request['production'])) {
    return FALSE;
  }
  
  return TRUE;
}

function cnapi_extras_block_view_movie_playlist() {
  $request = cnapi_ui_get_active_request();
  
  $production = cnapi_get_production($request['production'], CNAPI_LIST_DETAIL);
  
  $events = $production['related_events'];
  
  $movies = array();
  
  $locations = cnapi_get_locations_tree(CNAPI_LOCATIONS_DIMENSION_ADMINISTRATIVE, TRUE, 'flat');
  
  foreach ($events as $event) {
    $title = $event['detail']['nl']['title'];
    $city = $event['location']['address']['city'];
    $cityid = $event['location']['address']['cityid'];
    $locationid = $event['location']['actor']['cdbid'];
    $pid = $locations[$cityid]['pid'];
    $pid = $locations[$pid]['pid'];
    $province = $locations[$pid]['name'];

    $movies[$province][$locationid][] = $event;
  }
  
  $block['subject'] = 'Speellijst';
  $block['content'] = array(
    '#theme' => 'cnapi_extras_movie_playlist',
    '#movies' => $movies,
  );
  
  return $block;  
}

function cnapi_extras_block_cache_id_movie_results() {
  $request = cnapi_ui_get_active_request();
  
  $request = cnapi_url_dp2p($request);
  
  return array(cnapi_request_hash($request));
}

function cnapi_extras_block_is_visible_movie_results() {
  $request = cnapi_ui_get_active_request();
  
  if (isset($request['context'])) {
    $context = cnapi_ui_contexts('id', $request['context']);
    if ($context['type'] != 'event') {
      return FALSE;
    }
  }
  else {
    return FALSE;
  }

  if (isset($request['query']['heading'])) {
    return FALSE;
  }

  if (isset($request['query']['facilities']) && in_array($request['query']['facilities'], array('3.21.0.0.0', '3.12.0.0.0', '3.13.0.0.0', '3.12.0.0.0', '3.17.0.0.0', '3.14.0.0.0'))) {
    return FALSE;
  }

  if (isset($request['query']['isfree'])) {
    return FALSE;
  }

  if (isset($request['query']['type'])) {
    return FALSE;
  }

	if (isset($request['query']['thema']) && $request['query']['thema'] != '1.7.0.0.0') {
    return FALSE;
  }

  if (isset($request['query']['page']) && $request['query']['page'] > 0) {
    return FALSE;
  }
  
  return TRUE;
}

function cnapi_extras_block_view_movie_results() {
  $request_dp = cnapi_ui_get_active_request();
  
  $movie_context = cnapi_ui_contexts('id', 'movie');
  
  $context = $request_dp['context'];
  
  $request_p = cnapi_url_dp2p($request_dp);
  
  cnapi_ui_remove_context_query($request_p['query'], $context); // we only want the query the user was actualy doing (without extra context queries)

  $query = $request_p['query'];
  
  foreach ($movie_context['query'] as $key => $value) {
    $query[$key] = $value;
  }
  
  $query['pagelength'] = 5;

  $productions = cnapi_get_productions($query);

  if (empty($productions['data'])) {
    return;
  }
  
  unset($query['pagelength']);
  
  $request_p['query'] = $query;

  $request_dp = cnapi_url_p2dp($request_p);
  
  $more_request = array('context' => 'movie', 'query' => $request_dp['query']);

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

function cnapi_extras_block_cache_id_wider_radius_search() {
  $request = cnapi_ui_get_active_request();
  
  $request = cnapi_url_dp2p($request);
  
  unset($request['query']['page']);
  unset($request['query']['pagelength']);
  unset($request['query']['sort']);
  
  return array(cnapi_request_hash($request));
}

function cnapi_extras_block_is_visible_wider_radius_search() {
  // Get the current request.
  $request = cnapi_ui_get_active_request();

  // We only act upon cnapi requests.
  if (!$request || !isset($request['context'])) {
    return FALSE;
  }
  
  return TRUE;
}

/**
 * The wider radius block provides a block on cnapi search results pages that have no results.
 * If no results were found and the request contains a filter on city or cityid, we will iteratively search in a wider and wider radius from that city for possible results.
 * If we still can't find results within the maximum radius, we expand to the whole of Flanders.
 */
function cnapi_extras_block_view_wider_radius_search() {
  // Get the current request.
  $request = cnapi_ui_get_active_request();

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