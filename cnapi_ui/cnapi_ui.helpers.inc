<?php

function cnapi_ui_clean_invalid_queries(&$query) {
  $valid_queries_common = array('query', 'k', 'changedsince', 'zip', 'city', 'cityid', 'regio', 'latlng', 'format', 'sort', 'page', 'pagelength');
  $valid_queries_events_productions = array('locationkeyword', 'organiserkeyword', 'organiser', 'agebetween', 'age', 'isfree', 'isparent', 'permanent', 'temporary', 'location', 'agefrom', 'organizer', 'type', 'thema', 'targetaudience', 'facility', 'publicscope', 'locationtype', 'eventtype', 'actortype', 'municipal', 'ipe', 'misc', 'heading', 'daterange', 'date', 'datetype');
  
  $valid_queries = array_merge($valid_queries_common, $valid_queries_events_productions);
  $diff = array_diff(array_keys($query), $valid_queries);
  
  foreach ($diff as $key) {
    unset($query[$key]);
  }
}

function cnapi_ui_goto($request, $options = array(), $http_response_code = 302) {
  $url = cnapi_url_dp2dua($request);

  $path = $url['path'];
  $options = $url['options'];

  if (!empty($options)) {
    foreach ($options as $key => $value) {
      $options[$key] = $value;
    }
  }

  drupal_goto($path, $url['options'], $http_response_code);
}

function cnapi_ui_get_active_url($clean = TRUE) {
  $url = &drupal_static(__FUNCTION__, array());
  
  if (isset($url[$clean])) {
    return $url[$clean];
  }

  $path = $_GET['q'];
  $query = $_GET;
  unset($query['q']);
  
  if ($clean) {
    cnapi_ui_clean_invalid_queries($query);
  }

  $url[$clean] = array(
    'path' => $path,
    'options' => array(
      'query' => $query,
    ),
  );

  return $url[$clean];
}

function cnapi_ui_get_active_request($clean = TRUE) {
  $url = cnapi_ui_get_active_url($clean);

  if (module_exists('cnapi_snode')) {
    cnapi_snode_url_outbound_alter($url['path'], $url['options'], $url['path']);
  }

  return cnapi_url_dua2dp($url);
}

function cnapi_ui_get_active_context() {
  $request = cnapi_ui_get_active_request();

  return $request['context'];
}

/**
 * Calculate a slug with a maximum length for a string.
 *
 * @param $string
 *   The string you want to calculate a slug for.
 * @param $length
 *   The maximum length the slug can have.
 * @return
 *   A string representing the slug
 */
function cnapi_ui_slug($string, $length = 50, $separator = '-') {
  // transliterate
  $string = cnapi_ui_transliterate($string);

  // lowercase
  $string = strtolower($string);

  // replace non alphanumeric and non underscore charachters by separator
  $string = preg_replace('/[^a-z0-9]/i', $separator, $string);

  // replace multiple occurences of separator by one instance
  $string = preg_replace('/' . preg_quote($separator) . '[' . preg_quote($separator) . ']*/', $separator, $string);

  // cut off to maximum length
  if ($length > -1 && strlen($string) > $length) {
    $string = substr($string, 0, $length);
  }

  // remove separator from start and end of string
  $string = preg_replace('/' . preg_quote($separator) . '$/', '', $string);
  $string = preg_replace('/^' . preg_quote($separator) . '/', '', $string);

  return $string;
}

/**
 * Transliterate a given string.
 *
 * @param $string
 *   The string you want to transliterate.
 * @return
 *   A string representing the transliterated version of the input string.
 */
function cnapi_ui_transliterate($string) {
  static $charmap;
  if (!$charmap) {
    $charmap = array(
      // Decompositions for Latin-1 Supplement
      chr(195) . chr(128) => 'A', chr(195) . chr(129) => 'A',
      chr(195) . chr(130) => 'A', chr(195) . chr(131) => 'A',
      chr(195) . chr(132) => 'A', chr(195) . chr(133) => 'A',
      chr(195) . chr(135) => 'C', chr(195) . chr(136) => 'E',
      chr(195) . chr(137) => 'E', chr(195) . chr(138) => 'E',
      chr(195) . chr(139) => 'E', chr(195) . chr(140) => 'I',
      chr(195) . chr(141) => 'I', chr(195) . chr(142) => 'I',
      chr(195) . chr(143) => 'I', chr(195) . chr(145) => 'N',
      chr(195) . chr(146) => 'O', chr(195) . chr(147) => 'O',
      chr(195) . chr(148) => 'O', chr(195) . chr(149) => 'O',
      chr(195) . chr(150) => 'O', chr(195) . chr(153) => 'U',
      chr(195) . chr(154) => 'U', chr(195) . chr(155) => 'U',
      chr(195) . chr(156) => 'U', chr(195) . chr(157) => 'Y',
      chr(195) . chr(159) => 's', chr(195) . chr(160) => 'a',
      chr(195) . chr(161) => 'a', chr(195) . chr(162) => 'a',
      chr(195) . chr(163) => 'a', chr(195) . chr(164) => 'a',
      chr(195) . chr(165) => 'a', chr(195) . chr(167) => 'c',
      chr(195) . chr(168) => 'e', chr(195) . chr(169) => 'e',
      chr(195) . chr(170) => 'e', chr(195) . chr(171) => 'e',
      chr(195) . chr(172) => 'i', chr(195) . chr(173) => 'i',
      chr(195) . chr(174) => 'i', chr(195) . chr(175) => 'i',
      chr(195) . chr(177) => 'n', chr(195) . chr(178) => 'o',
      chr(195) . chr(179) => 'o', chr(195) . chr(180) => 'o',
      chr(195) . chr(181) => 'o', chr(195) . chr(182) => 'o',
      chr(195) . chr(182) => 'o', chr(195) . chr(185) => 'u',
      chr(195) . chr(186) => 'u', chr(195) . chr(187) => 'u',
      chr(195) . chr(188) => 'u', chr(195) . chr(189) => 'y',
      chr(195) . chr(191) => 'y',
      // Decompositions for Latin Extended-A
      chr(196) . chr(128) => 'A', chr(196) . chr(129) => 'a',
      chr(196) . chr(130) => 'A', chr(196) . chr(131) => 'a',
      chr(196) . chr(132) => 'A', chr(196) . chr(133) => 'a',
      chr(196) . chr(134) => 'C', chr(196) . chr(135) => 'c',
      chr(196) . chr(136) => 'C', chr(196) . chr(137) => 'c',
      chr(196) . chr(138) => 'C', chr(196) . chr(139) => 'c',
      chr(196) . chr(140) => 'C', chr(196) . chr(141) => 'c',
      chr(196) . chr(142) => 'D', chr(196) . chr(143) => 'd',
      chr(196) . chr(144) => 'D', chr(196) . chr(145) => 'd',
      chr(196) . chr(146) => 'E', chr(196) . chr(147) => 'e',
      chr(196) . chr(148) => 'E', chr(196) . chr(149) => 'e',
      chr(196) . chr(150) => 'E', chr(196) . chr(151) => 'e',
      chr(196) . chr(152) => 'E', chr(196) . chr(153) => 'e',
      chr(196) . chr(154) => 'E', chr(196) . chr(155) => 'e',
      chr(196) . chr(156) => 'G', chr(196) . chr(157) => 'g',
      chr(196) . chr(158) => 'G', chr(196) . chr(159) => 'g',
      chr(196) . chr(160) => 'G', chr(196) . chr(161) => 'g',
      chr(196) . chr(162) => 'G', chr(196) . chr(163) => 'g',
      chr(196) . chr(164) => 'H', chr(196) . chr(165) => 'h',
      chr(196) . chr(166) => 'H', chr(196) . chr(167) => 'h',
      chr(196) . chr(168) => 'I', chr(196) . chr(169) => 'i',
      chr(196) . chr(170) => 'I', chr(196) . chr(171) => 'i',
      chr(196) . chr(172) => 'I', chr(196) . chr(173) => 'i',
      chr(196) . chr(174) => 'I', chr(196) . chr(175) => 'i',
      chr(196) . chr(176) => 'I', chr(196) . chr(177) => 'i',
      chr(196) . chr(178) => 'IJ', chr(196) . chr(179) => 'ij',
      chr(196) . chr(180) => 'J', chr(196) . chr(181) => 'j',
      chr(196) . chr(182) => 'K', chr(196) . chr(183) => 'k',
      chr(196) . chr(184) => 'k', chr(196) . chr(185) => 'L',
      chr(196) . chr(186) => 'l', chr(196) . chr(187) => 'L',
      chr(196) . chr(188) => 'l', chr(196) . chr(189) => 'L',
      chr(196) . chr(190) => 'l', chr(196) . chr(191) => 'L',
      chr(197) . chr(128) => 'l', chr(197) . chr(129) => 'L',
      chr(197) . chr(130) => 'l', chr(197) . chr(131) => 'N',
      chr(197) . chr(132) => 'n', chr(197) . chr(133) => 'N',
      chr(197) . chr(134) => 'n', chr(197) . chr(135) => 'N',
      chr(197) . chr(136) => 'n', chr(197) . chr(137) => 'N',
      chr(197) . chr(138) => 'n', chr(197) . chr(139) => 'N',
      chr(197) . chr(140) => 'O', chr(197) . chr(141) => 'o',
      chr(197) . chr(142) => 'O', chr(197) . chr(143) => 'o',
      chr(197) . chr(144) => 'O', chr(197) . chr(145) => 'o',
      chr(197) . chr(146) => 'OE', chr(197) . chr(147) => 'oe',
      chr(197) . chr(148) => 'R', chr(197) . chr(149) => 'r',
      chr(197) . chr(150) => 'R', chr(197) . chr(151) => 'r',
      chr(197) . chr(152) => 'R', chr(197) . chr(153) => 'r',
      chr(197) . chr(154) => 'S', chr(197) . chr(155) => 's',
      chr(197) . chr(156) => 'S', chr(197) . chr(157) => 's',
      chr(197) . chr(158) => 'S', chr(197) . chr(159) => 's',
      chr(197) . chr(160) => 'S', chr(197) . chr(161) => 's',
      chr(197) . chr(162) => 'T', chr(197) . chr(163) => 't',
      chr(197) . chr(164) => 'T', chr(197) . chr(165) => 't',
      chr(197) . chr(166) => 'T', chr(197) . chr(167) => 't',
      chr(197) . chr(168) => 'U', chr(197) . chr(169) => 'u',
      chr(197) . chr(170) => 'U', chr(197) . chr(171) => 'u',
      chr(197) . chr(172) => 'U', chr(197) . chr(173) => 'u',
      chr(197) . chr(174) => 'U', chr(197) . chr(175) => 'u',
      chr(197) . chr(176) => 'U', chr(197) . chr(177) => 'u',
      chr(197) . chr(178) => 'U', chr(197) . chr(179) => 'u',
      chr(197) . chr(180) => 'W', chr(197) . chr(181) => 'w',
      chr(197) . chr(182) => 'Y', chr(197) . chr(183) => 'y',
      chr(197) . chr(184) => 'Y', chr(197) . chr(185) => 'Z',
      chr(197) . chr(186) => 'z', chr(197) . chr(187) => 'Z',
      chr(197) . chr(188) => 'z', chr(197) . chr(189) => 'Z',
      chr(197) . chr(190) => 'z', chr(197) . chr(191) => 's',
      // Euro Sign
      chr(226) . chr(130) . chr(172) => 'E'
    );
  }

  // transliterate
  return strtr($string, $charmap);
}

/**
 * Check if a certain string is a slug.
 *
 * @param $str
 *   The string you want to see for if it's a slug.
 * @return
 *   Boolean value: TRUE if the specified string is a slug; FALSE if otherwise.
 */
function cnapi_ui_is_slug($str) {
  return $str == cnapi_ui_slug($str);
}