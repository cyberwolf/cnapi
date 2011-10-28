<?php

/**
 * @file
 * Preprocessing.
 */

/**
 * Preprocess variables for page.tpl.php.
 *
 * We want to override the page title ($title) on list (context) pages to use the context title as the page title.
 *
 * @see page.tpl.php
 */
function cnapi_ui_preprocess_page(&$variables) {
  // Get the current request.
  $request = cnapi_ui_get_active_request();

  // We only act upon list pages.
  if (!isset($request['context'])) {
    return;
  }

  // Fetch the full context object to determine the title.
  $context = cnapi_ui_contexts('id', $request['context']);
}

/**
 * Preprocess variables for cnapi-ui-list.tpl.php.
 *
 * @see cnapi-ui-list.tpl.php
 */
function template_preprocess_cnapi_ui_list(&$variables) {
  // Text for "no results".
  $variables['no_results'] = 'Geen resultaten gevonden';

  // If we have a free text search "filter", we add it together with the "remove filter url" to the result info.
  $filters = cnapi_ui_get_active_request_filters();

  if (isset($filters['query'])) {
    $query = '<strong>' . $filters['query'][0]['description'] . ' ' . cnapi_url_dp2dul('x', $filters['query'][0]['url_remove']) . '</strong>';
  }

  // Text for result info (number of results, ...)
  if (isset($query)) {
    $variables['result_info'] = format_plural($variables['total'], '<strong>@total result</strong> found for <strong>!query</strong>', '<strong>@total results</strong> found for <strong>!query</strong>', array('@total' => $variables['total'], '!query' => $query));
  }
  else {
    $variables['result_info'] = format_plural($variables['total'], '<strong>@total result</strong> found', '<strong>@total results</strong> found', array('@total' => $variables['total']));
  }
  
  // Result counter
  global $pager_page_array, $pager_total, $pager_total_items, $pager_limits;

  $start = $pager_page_array[0] * $pager_limits[0] + 1;
  $stop = min(($pager_page_array[0] + 1) * $pager_limits[0], $pager_total_items[0]);
  $total = $pager_total_items[0];
  $variables['result_counter'] = sprintf('%d-%d van %d', $start, $stop, $total);

  // Set the $objects variable as an array of themed object summaries.
  $variables['objects'] = array();
  foreach ($variables['items'] as $object) {
    $variables['objects'][] = theme('cnapi_ui_' . $variables['type'] . '_summary', array($variables['type'] => $object));
  }

  // Gather the sort links and add them as an array to $sort_links.
  $sort_links = array();

  $request_sort = $variables['request'];
  
  $context = cnapi_ui_contexts('id', $variables['request']['context']);
  
  if ($context['type'] == 'event') {
    $request_sort['query']['sort'] = 'date ASC';
    $sort_links[] = cnapi_url_dp2dul(t('By date'), $request_sort);
    
    $request_sort['query']['sort'] = 'location ASC';
    $sort_links[] = cnapi_url_dp2dul(t('By location'), $request_sort);
    
    $request_sort['query']['sort'] = 'title ASC';
    $sort_links[] = cnapi_url_dp2dul(t('By title'), $request_sort);
  }
  elseif ($context['type'] == 'production') {
    $request_sort['query']['sort'] = 'weight ASC';
    $sort_links[] = cnapi_url_dp2dul(t('By most plays'), $request_sort);
    
    $request_sort['query']['sort'] = 'title ASC';
    $sort_links[] = cnapi_url_dp2dul(t('By title'), $request_sort);
  }

  if (!empty($sort_links)) {
    $variables['sort_links'] = theme('item_list', array('items' => $sort_links));
  }

  // Add a link to the RSS feed in $rss.
  $url_rss = cnapi_ui_rss_feed_url_raw();
  $variables['rss'] = l(t('RSS'), $url_rss['path'], $url_rss['options']);

  // Add the pager in $pager.
  $variables['pager'] = theme('pager');
}

/**
 * Preprocess variables for cnapi-ui-actor.tpl.php.
 *
 * @see cnapi-ui-actor.tpl.php
 */
function template_preprocess_cnapi_ui_actor_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'actor');
}

/**
 * Preprocess variables for cnapi-ui-event-mini-summary.tpl.php.
 *
 * Actual preprocessing is delegated to _template_preprocess_cnapi_ui_object_summary.
 *
 * @see cnapi-ui-event-mini-summary.tpl.php
 */
function template_preprocess_cnapi_ui_event_mini_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'event');
}

/**
 * Preprocess variables for cnapi-ui-event.tpl.php.
 *
 * Actual preprocessing is delegated to _template_preprocess_cnapi_ui_object_summary.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_event_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'event');
}

/**
 * Preprocess variables for cnapi-ui-production.tpl.php.
 *
 * Actual preprocessing is delegated to _template_preprocess_cnapi_ui_object_summary.
 *
 * @see cnapi-ui-production.tpl.php
 */
function template_preprocess_cnapi_ui_production_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'production');
}

/**
 * Helper for preprocessing variables common between cnapi_ui_actor_summary, cnapi_ui_event_summary, cnapi_ui_production_summary.
 */
function _template_preprocess_cnapi_ui_object_summary(&$variables, $type) {
  $object = $variables[$type];

  // cdbid
  $variables['cdbid'] = $object['cdbid'];

  // title
  $variables['title'] = check_plain($object['title']);

  // object_url
  $request_detail = array($type => $object['cdbid'], 'title' => $object['title']);
  $object_url = cnapi_url_dp2dua($request_detail);
  $object_url['options']['absolute'] = $variables['url_absolute'];

  $variables['object_url'] = url($object_url['path'], $object_url['options']);

  // more_link
  $variables['more_link'] = l('Meer info', $object_url['path'], $object_url['options']);

  // thumbnail
  $default_image = url(drupal_get_path('module', 'cnapi_ui') . '/img/default-image.gif', array('absolute' => $variables['url_absolute']));
  $thumbnail_url = isset($object['thumbnail']) && !empty($object['thumbnail']) ? $object['thumbnail'] : $default_image;
  $thumbnail_img = theme('image', array('path' => $thumbnail_url, 'alt' => $variables['title']));
  $object_url['options']['html'] = TRUE;
  $variables['thumbnail'] = l($thumbnail_img, $object_url['path'], $object_url['options']);

  // shortdescription
  $variables['shortdescription'] = isset($object['shortdescription']) && !empty($object['shortdescription']) ? truncate_utf8(strip_tags($object['shortdescription']), 200, TRUE, TRUE) : '';

  // performers
  if ($type == 'event' || $type == 'production') {
    $variables['performers'] = '';

    if (isset($object['performers'])) {
      $performers = array_slice($object['performers'], 0, 3);
      $variables['performers'] = implode(', ', $performers);

      if (count($object['performers']) > 3) {
        $variables['performers'] .= ',&nbsp;&hellip;';
      }
    }
  }

  // vlieg
  if ($type == 'event' || $type == 'production') {
    _template_preprocess_cnapi_ui_object_vlieg($variables, $type);
  }

  // agefrom
  if ($type == 'event' || $type == 'production') {
    $variables['agefrom'] = isset($object['agefrom']) ? $object['agefrom'] : '';
  }

  // where
  if ($type == 'event') {
    $where = array();
    if (isset($object['location']) && !empty($object['location'])) {
      $where[] = check_plain($object['location']);
    }
    if (isset($object['city']) && !empty($object['city'])) {
      $where[] = check_plain($object['city']);
    }
    $variables['where'] = !empty($where) ? implode(', ', $where) : '';
  }

  // when
  if ($type == 'event') {
    $variables['when'] = isset($object['calendarsummary']) ? $object['calendarsummary'] : '';
  }

	// headings
  if ($type == 'event') {
    $variables['headings'] = '';

    $headings = cnapi_get_headings();
    if (!empty($object['headingid'])) {
      foreach ($object['headingid'] as $heading) {;
        $heading_info = $headings[ltrim($heading)];
        if (!cnapi_output_type_has_heading_groups() || (cnapi_output_type_has_heading_groups() && $heading_info['pid'] != 0)) {
          $variables['headings'][] = $heading_info['name'];
        }
      }
      if (!empty($variables['headings'])) {
        $variables['headings'] = implode(', ', $variables['headings']);
      }
    }
  }
  
}

/**
 * Preprocess variables for cnapi-ui-actor.tpl.php.
 *
 * Actual preprocessing is delegated to _template_preprocess_cnapi_ui_object.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_actor(&$variables) {
  _template_preprocess_cnapi_ui_object($variables, 'actor');
}

/**
 * Preprocess variables for cnapi-ui-event.tpl.php.
 *
 * Actual preprocessing is delegated to _template_preprocess_cnapi_ui_object.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_event(&$variables) {
  _template_preprocess_cnapi_ui_object($variables, 'event');
}

/**
 * Preprocess variables for cnapi-ui-production.tpl.php.
 *
 * Actual preprocessing is delegated to _template_preprocess_cnapi_ui_object.
 *
 * @see cnapi-ui-production.tpl.php
 */
function template_preprocess_cnapi_ui_production(&$variables) {
  _template_preprocess_cnapi_ui_object($variables, 'production');
}

/**
 * Helper for preprocessing variables common between cnapi_ui_actor, cnapi_ui_event, cnapi_ui_production.
 */
function _template_preprocess_cnapi_ui_object(&$variables, $type) {
  $object = $variables[$type];

  $detail = $object['detail']['nl'];

  // cdbid
  $variables['cdbid'] = $object['cdbid'];

  // title
  $variables['title'] = check_plain($detail['title']);

  // shortdescription
  $variables['shortdescription'] = isset($detail['shortdescription']) && !empty($detail['shortdescription']) ? strip_tags($detail['shortdescription']) : '';

  // longdescription
  $variables['longdescription'] = isset($detail['longdescription']) && !empty($detail['longdescription']) ? $detail['longdescription'] : '';

  // images
  $variables['images'] = array();

  $media = isset($detail['media']['other']) ? $detail['media']['other'] : array();
  if (isset($detail['media']['main'])) {
    $media[] = $detail['media']['main'];
  }

  foreach ($media as $media_item) {
    $image = array();

    // title
    if (isset($media_item['title'])) {
      $image['title'] = $media_item['title'];
    }

    // image
    if (isset($media_item['hlink']) && isset($media_item['mediatype']) && in_array($media_item['mediatype'], array('photo', 'imageweb'))) {
      $vars = array('path' => $media_item['hlink'] . '?maxwidth=240');
      if (isset($image['title'])) {
        $vars['alt'] = $image['title'];
        $vars['title'] = $image['title'];
      }
      $image['image'] = theme('image', $vars);
    }

    // copyright
    if (isset($media_item['copyright'])) {
      $image['copyright'] = $media_item['copyright'];
    }

    if (isset($image['image'])) {
      $variables['images'][] = $image;
    }
  }

  // headings
  if ($type == 'event') {
    $variables['headings'] = '';

    $headings = cnapi_get_headings();
    if (isset($object['headings'])) {
      foreach ($object['headings'] as $i => $heading) {
        $heading_info = $headings[$heading];
        if (!cnapi_output_type_has_heading_groups() || (cnapi_output_type_has_heading_groups() && $heading_info['pid'] != 0)) {
          $variables['headings'][] = $heading_info['name'];
        }
      }
    }
  }

  // price
  if ($type == 'event') {
    $variables['price'] = '';
    if (isset($detail['price']['value'])) {
      $variables['price'] = $detail['price']['value'] == '0' ? 'Gratis' : '&euro; '. $detail['price']['value'];
      $variables['price'] .= isset($detail['price']['description']) ? ' (' . $detail['price']['description'] . ')' : '';
    }
    elseif (isset($detail['price']['description'])) {
      $variables['price'] = $detail['price']['description'];
    }
  }

  // vlieg
  if ($type == 'event' || $type == 'production') {
    _template_preprocess_cnapi_ui_object_vlieg($variables, $type);
  }

  // agefrom
  if ($type == 'event' || $type == 'production') {
    $variables['agefrom'] = isset($object['agefrom']) ? $object['agefrom'] . '+' : '';
  }

  // when
  if ($type == 'actor' || $type == 'event') {
    $variables['when'] = isset($detail['calendarsummary']) ? $detail['calendarsummary'] : '';
  }

  // performers
  if ($type == 'event') {
    $variables['performers'] = array();
    if (isset($detail['performers'])) {
      $performers = array();
      foreach ($detail['performers'] as $performer) {
        $role = isset($performer['role']) ? ' (' . $performer['role'] . ')' : '';
        $variables['performers'][] = $performer['name'] . $role;
      }
    }
  }

  if ($type == 'production') { // @todo this should only be done on movie productions
    $variables['performers'] = array();
    $variables['directors'] = array();
    $variables['actors'] = array();

    if (isset($detail['performers'])) {
      $performers = array();
      foreach ($detail['performers'] as $performer) {
        $role = isset($performer['role']) ? $performer['role'] : '';

        // actors
        if ($role && in_array(strtolower($role), array('actor', 'acteur', 'actrice'))) {
          $variables['actors'][] = $performer['name'];
        }

        // directors
        elseif ($role && in_array(strtolower($role), array('director', 'regisseur'))) {
          $variables['directors'][] = $performer['name'];
        }

        // other performers
        else {
          $role = $role ? ' (' . $role . ')' : '';
          $variables['performers'][] = $performer['name'] . $role;
        }
      }
    }
  }

  // location
  if ($type == 'event') {
    $variables['location'] = array();

    if (isset($object['location'])) {
      $location = $object['location'];

      // location address
      if (isset($location['address'])) {
        $variables['location']['address'] = _template_preprocess_cnapi_ui_address($location['address']);
      }

      // location title
      if (isset($location['actor']['detail']['nl']['title'])) {
        $variables['location']['title'] = $location['actor']['detail']['nl']['title'];
      }
      
      elseif (isset($location['location'])) {
        $variables['location']['title'] = $location['location'];
      }

      // location link
      if (isset($location['actor']['asset']) && $location['actor']['asset']) {
        $request_actor = array('actor' => $location['actor']['cdbid'], 'title' => $variables['location']['title']);
        $variables['location']['link'] = cnapi_url_dp2dul($variables['location']['title'], $request_actor);
      }
    }
  }

  // address
  if ($type == 'actor') {
    $variables['address'] = '';

    if (isset($object['contactinfo']['address'])) {
      $variables['address'] = _template_preprocess_cnapi_ui_address($object['contactinfo']['address']);
    }
  }

  // organiser
  $variables['organiser'] = array();

  if (isset($object['organiser'])) {
    $organiser = $object['organiser'];

    // organiser title
    if (isset($organiser['detail']['nl']['title'])) {
      $variables['organiser']['title'] = $organiser['detail']['nl']['title'];
    }

    // organiser link
    if (isset($organiser['asset']) && $organiser['asset']) {
      $request_actor = array('actor' => $organiser['cdbid'], 'title' => $variables['organiser']['title']);
      $variables['organiser']['link'] = cnapi_url_dp2dul($variables['organiser']['title'], $request_actor);
    }
  }

  // contact
  $variables['contact'] = array();

  if (isset($object['contactinfo']['main']) || isset($object['contactinfo']['other'])) {
    $contact = array();

    // url, mail, phone, fax will be taken from contacinfo/main. If it wasn't found there, we also look at contactinfo/other.
    if (isset($object['contactinfo']['main'])) {
      $contact = $object['contactinfo']['main'];
    }

    if (isset($object['contactinfo']['other'])) {
      $contact_types = array_keys($object['contactinfo']['other']);
      foreach ($contact_types as $contact_type) {
        $contact[$contact_type] = isset($contact[$contact_type]) ? array($contact[$contact_type]) : $object['contactinfo']['other'][$contact_type];
      }
    }

    // mail
    if (isset($contact['mail'])) {
      foreach ($contact['mail'] as $mail) {
        $variables['contact']['mail'][] = l($mail, 'mailto:' . $mail);
      }
      $variables['contact']['mail'] = implode(', ', $variables['contact']['mail']);
    }

    // phone
    if (isset($contact['phone'])) {
      foreach ($contact['phone'] as $phone) {
        $variables['contact']['phone'][] = $phone;
      }
      $variables['contact']['phone'] = implode(', ', $variables['contact']['phone']);
    }

    // fax
    if (isset($contact['fax'])) {
      foreach ($contact['fax'] as $fax) {
        $variables['contact']['fax'][] = $fax;
      }
      $variables['contact']['fax'] = implode(', ', $variables['contact']['fax']);
    }
  }

  // coords
  if ($type == 'actor' || $type == 'event') {
    $variables['coords'] = array();

    if (isset($object['contactinfo']['address']['gis']['lat']) && isset($object['contactinfo']['address']['gis']['lng'])) {
      $variables['coords'] = array(
        'lat' => $object['contactinfo']['address']['gis']['lat'],
        'lng' => $object['contactinfo']['address']['gis']['lng'],
      );
    }
  }

  // reservation
  if ($type == 'actor' || $type == 'event') {
    $variables['reservation'] = array();

    if (isset($object['contactinfo']['reservation'])) {
      $reservation = $object['contactinfo']['reservation'];

      // mail
      if (isset($reservation['mail'])) {
        $variables['reservation']['mail'] = l($reservation['mail'], 'mailto:' . $reservation['mail']);
      }

      // phone
      if (isset($reservation['phone'])) {
        $variables['reservation']['phone'] = $reservation['phone'];
      }

      // fax
      if (isset($reservation['fax'])) {
        $variables['reservation']['fax'] = $reservation['fax'];
      }
    }
  }

  // is_only_french
  if ($type == 'event') {
    $variables['is_only_french'] = FALSE;

    if (isset($object['languages']) && count($object['languages']) == 1) {
      foreach ($object['languages'] as $language) {
        if (in_array(strtolower($language['name']), array('fr', 'frans'))) {
          $variables['is_only_french'] = TRUE;
        }
      }
    }
  }

  // keywords
  if ($type == 'event' || $type == 'production') {
    $variables['keywords'] = array();

    if (isset($object['keywords'])) {
      foreach ($object['keywords'] as $keyword) {
        $request_keyword = array('context' => $type, 'query' => array('k' => str_replace(' ', '_', $keyword)));

        $variables['keywords'][] = cnapi_url_dp2dul($keyword, $request_keyword);
      }
    }
  }

  // targetaudiences
  if ($type == 'event' || $type == 'production') {
    $variables['targetaudiences'] = array();

    if (isset($object['categories']['targetaudience'])) {
      $targetaudiences = cnapi_get_age_types() + cnapi_get_targetaudiences();

      foreach ($object['categories']['targetaudience'] as $audience) {
        $request_audience = array('context' => $type, 'query' => array('targetaudience' => $audience));
        $variables['targetaudiences'][] = cnapi_url_dp2dul($targetaudiences[$audience]['name'], $request_audience);
      }
    }
  }

  // facilities
  if ($type == 'event' || $type == 'production') {
    $variables['facilities'] = array();

    if (isset($object['categories']['facility'])) {
      $targetaudiences = cnapi_get_facilities();

      foreach ($object['categories']['facility'] as $facility) {
        $request_facility = array('context' => $type, 'query' => array('facility' => $facility));
        $variables['facilities'][] = cnapi_url_dp2dul($facilities[$facility]['name'], $request_facility);
      }
    }
  }

  // tickets
  if ($type == 'event') {
    $variables['tickets'] = array();
    if (isset($detail['media']['other'])) {
      foreach ($detail['media']['other'] as $link) {
        if (isset($link['mediatype']) && $link['mediatype'] == 'reservations') {
          $variables['tickets'][] = l($link['title'], $link['hlink']);
        }
      }
    }
  }

  // links
  if ($type == 'event') {
    $variables['links'] = array();

    $links = array();

    // event links
    if (isset($object['contactinfo']['main']['url'])) {
      $links[] = $object['contactinfo']['main']['url'];
    }

    if (isset($object['contactinfo']['other']['url'])) {
      foreach ($object['contactinfo']['other']['url'] as $url) {
        $links[] = $url;
      }
    }

    // location links
    if (isset($object['location']['actor']['contactinfo']['main']['url'])) {
      $links[] = $object['location']['actor']['contactinfo']['main']['url'];
    }

    if (isset($object['location']['actor']['contactinfo']['other']['url'])) {
      foreach ($object['location']['actor']['contactinfo']['other']['url'] as $url) {
        $links[] = $url;
      }
    }

    // organiser links
    if (isset($object['organiser']['contactinfo']['main']['url'])) {
      $links[] = $object['organiser']['contactinfo']['main']['url'];
    }

    if (isset($object['organiser']['contactinfo']['other']['url'])) {
      foreach ($object['organiser']['contactinfo']['other']['url'] as $url) {
        $links[] = $url;
      }
    }

    // performer links
    if (isset($detail['performers']['actor']['contactinfo']['main']['url'])) {
      $links[] = $detail['performers']['actor']['contactinfo']['main']['url'];
    }

    if (isset($detail['performers']['actor']['contactinfo']['other']['url'])) {
      foreach ($detail['performers']['actor']['contactinfo']['other']['url'] as $url) {
        $links[] = $url;
      }
    }

    // cleaning up links
    foreach ($links as $i => $link) {
      $links[$i] = rtrim($link, '/');
    }

    $links = array_unique($links);

    foreach ($links as $link) {
      $variables['links'][] = l(truncate_utf8($link, 30, FALSE, TRUE), $link);
    }
  }
}

/**
 * Helper for preprocessing performers in cnapi_ui_event_summary, cnapi_ui_project_summary.
 */
function _template_preprocess_cnapi_ui_object_vlieg(&$variables, $type) {
  $object = $variables[$type];

  // In case url_absolute was not passed, we default to FALSE.
  if (!isset($variables['url_absolute'])) {
    $variables['url_absolute'] = FALSE;
  }

  // vlieg image
  $vlieg_description = 'Hallo, ik ben Vlieg en ik wijs de weg naar leuke activiteiten voor kinderen. Meer info op www.vliegjemee.be!';

  $vlieg_image_url = url(drupal_get_path('module', 'cnapi_ui') . '/img/vlieg.jpg', array('absolute' => $variables['url_absolute']));
  $variables['vlieg_image'] = theme('image', array('path' => $vlieg_image_url, 'alt' => $vlieg_description, 'title' => $vlieg_description, 'attributes' => array('class' => 'vlieg-image')));

  // vlieg
  $variables['for_children'] = FALSE;

  // for_children
  if (isset($object['agefrom'])) {
    $variables['for_children'] = $object['agefrom'] < 12;;
  }
}

/**
 * Helper for generating an address string ([treet] [nr], [zip] [city]) from the address parts in an address object.
 */
function _template_preprocess_cnapi_ui_address($address) {
  $parts = $address_street = $address_city = array();

  if (isset($address['street'])) $address_street[] =  $address['street'];
  if (isset($address['housenr'])) $address_street[] = $address['housenr'];

  if (isset($address['zipcode'])) $address_city[] = $address['zipcode'];
  if (isset($address['city'])) $address_city[] = $address['city'];

  if (!empty($address_street)) $parts[] = trim(implode(' ', $address_street));
  if (!empty($address_city)) $parts[] = trim(implode(' ', $address_city));

  if (!empty($parts)) {
    return implode(', ', $parts);
  }

  return '';
}