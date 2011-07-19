<?php

function theme_cnapi_ui_page_list_rss($variables) {
  $objects = $variables['objects'];
  $type = $variables['type'];

  $items = array();
  if ($objects) {
    foreach ($objects as $object) {
      // object_url
      $request_detail = array($type => $object['cdbid'], 'title' => $object['title']);

      $object_url = cnapi_url_dp2dua($request_detail);
      $object_url['options']['absolute'] = TRUE;
      $object_url = url($object_url['path'], $object_url['options']);

      $args = array(
        array('key' => 'pubDate', 'value' => date('r', $object['created'])),
        array('key' => 'guid', 'value' => $object_url, 'attributes' => array('isPermaLink' => 'true')),
        array('key' => 'postid', 'value' => $object['cdbid']),
      );

      $body = theme('cnapi_ui_' . $type . '_summary', array($type => $object, 'url_absolute' => TRUE));
      $items .= format_rss_item($object['title'], $object_url, $body, $args);
    }
  }

  $site_name = variable_get('site_name', 'Drupal');
  $url['options']['absolute'] = TRUE;
  $url = url($url['path'], $url['options']);
  $description = '';

  $output  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  $output .= "<rss version=\"2.0\">\n";
  $output .= format_rss_channel(t('@site_name', array('@site_name' => $site_name)), $url, $description, $items);
  $output .= "</rss>\n";

  return $output;
}