(function ($) {

Drupal.cnapi_tracking = Drupal.cnapi_tracking || {};

Drupal.cnapi_tracking.track = function(activity, params) {
  params = params || {}
  
  var settings = Drupal.settings.cnapi_tracking;
  
  cultuurnet.track(settings.partnerKey, settings.user, activity, params);
}

})(jQuery);