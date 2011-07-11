<?php

/**
 * Preprocess variables for cnapi-ui-list.tpl.php.
 *
 * @see cnapi-ui-list.tpl.php
 */
function template_preprocess_cnapi_ui_list(&$variables) {
  // no results text
  $variables['no_results'] = t('No results found.');

  if (isset($variables['request']['query']['query'])) {
    $query = $variables['request']['query']['query'];

    $request_query_remove = $variables['request'];
    unset($request_query_remove['query']['query']);
    $query_remove_link = cnapi_url_dp2dul('x', $request_query_remove);

    $query = '<strong>' . $query . ' ' . $query_remove_link . '</strong>';
  }

  // result info
  if (isset($query)) {
    $variables['result_info'] = format_plural($variables['total'], '@total result found for !query', '@total results found for !query', array('@total' => $variables['total'], '!query' => $query));
  }
  else {
    $variables['result_info'] = format_plural($variables['total'], '@total result found', '@total results found', array('@total' => $variables['total']));
  }

  // objects
  $variables['objects'] = array();
  foreach ($variables['items'] as $object) {
    $variables['objects'][] = theme('cnapi_ui_' . $variables['type'] . '_summary', array($variables['type'] => $object));
  }

  // sort links
  $sort_links = array();

  $request_sort = $variables['request'];

  $request_sort['query']['sort'] = 'date ASC';
  $sort_links[] = cnapi_url_dp2dul(t('By date'), $request_sort);

  $request_sort['query']['sort'] = 'location ASC';
  $sort_links[] = cnapi_url_dp2dul(t('By location'), $request_sort);

  $request_sort['query']['sort'] = 'title ASC';
  $sort_links[] = cnapi_url_dp2dul(t('By title'), $request_sort);

  $variables['sort_links'] = theme('item_list', array('items' => $sort_links));

  // rss
  $url_rss = cnapi_ui_rss_feed_url_raw();
  $variables['rss'] = l(t('RSS'), $url_rss['path'], $url_rss['options']);

  // pager
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
 * Preprocess variables for cnapi-ui-event.tpl.php.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_event_summary(&$variables) {
  _template_preprocess_cnapi_ui_object_summary($variables, 'event');
}

/**
 * Preprocess variables for cnapi-ui-production.tpl.php.
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
  $variables['more_link'] = l(t('More info'), $object_url['path'], $object_url['options']);

  // thumbnail
  $default_image = url(drupal_get_path('module', 'cnapi_ui') . '/img/default-image.jpg', array('absolute' => $variables['url_absolute']));
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
    $variables['when'] = isset($event['calendarsummary']) ? $event['calendarsummary'] : '';
  }
}

/**
 * Preprocess variables for cnapi-ui-actor.tpl.php.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_actor(&$variables) {
  _template_preprocess_cnapi_ui_object($variables, 'actor');
}

/**
 * Preprocess variables for cnapi-ui-event.tpl.php.
 *
 * @see cnapi-ui-event.tpl.php
 */
function template_preprocess_cnapi_ui_event(&$variables) {
  _template_preprocess_cnapi_ui_object($variables, 'event');
}

/**
 * Preprocess variables for cnapi-ui-production.tpl.php.
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
  
  if ($type == 'production') {
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
          $variables['actors'][] = $performer['name'];
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

      // location link
      if (isset($location['actor']['asset']) && $location['actor']['asset']) {
        $request_actor = array('actor' => $location['actor']['cdbid'], 'title' => $variables['location']['title']);
        $variables['location']['link'] = cnapi_url_dp2dul($variables['location']['title'], $request_actor);
      }
    }
  }
  
  if ($type == 'actor') {
    $variables['address'] = '';
    
    // address
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

// links
// production : director / actor

// also look at https://cultuurnet.beanstalkapp.com/m-uitinvlaanderen-be/browse/trunk/sites/all/modules/custom/cnapi_mobile/cnapi_mobile.module
// also look at https://cultuurnet.beanstalkapp.com/m-uitinvlaanderen-be/browse/trunk/sites/all/modules/custom/cnapi_mobile/cnapi-mobile-event.tpl.php

/*

 // performers
  $vars['directors'] = '';
  $vars['actors'] = '';
  $vars['performers'] = '';
  if (isset($vars['production']['detail']['nl']['performers'])) {
    $directors = array();
    $actors = array();
    $performers = array();
    foreach($vars['production']['detail']['nl']['performers'] as $performer) {
      if(isset($performer['role'])) {
        if(($performer['role'] == 'acteur') or ($performer['role'] == 'actrice') or ($performer['role'] == 'Acteur') or ($performer['role'] == 'Actrice')) $actors[] = $performer['name'];
        if(($performer['role'] == 'regisseur') or ($performer['role'] == 'director') or ($performer['role'] == 'Regisseur')) $directors[] = $performer['name'];
      }
      else $performers[] = $performer['name'];
    }
    $vars['directors'] = implode(', ',$directors);
    $vars['actors'] = implode(', ',$actors);
    $vars['performers'] = implode(', ',$performers);
  }



  <?php if ($directors) : ?>
  <div class="lainwrap">
    <div class="label"><?php print t('Regie') ?></div>
    <div class="info"><?php print $directors; ?></div>
  </div>
  <?php endif; ?>

  <?php if ($actors) : ?>
  <div class="lainwrap">
    <div class="label"><?php print t('Acteurs') ?></div>
    <div class="info"><?php print $actors; ?></div>
  </div>
  <?php endif; ?>

  <?php if ($performers) : ?>
  <div class="lainwrap">
    <div class="label"><?php print t('Uitvoerders') ?></div>
    <div class="info"><?php print $performers; ?></div>
  </div>
  <?php endif; ?>




*/