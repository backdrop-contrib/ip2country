/**
 * @file
 * jQuery code for ip2country module user interface.
 */
(function ($) {  // Put definition of $ into local scope so jQuery code
                 // doesn't conflict with other JavaScript libraries.
  Backdrop.behaviors.ip2country = {
    attach: function() {
      // Remove focus from the "Update" button.
      $('#ip2country-admin-settings .form-submit').eq(1).focus();
      var selectCountry = function() {
        $('#edit-test-country').removeAttr('disabled');
        $('#edit-test-ip-address').attr('disabled', 'disabled');
      }
      var selectIP = function() {
        $('#edit-test-country').attr('disabled', 'disabled');
        $('#edit-test-ip-address').removeAttr('disabled');
      }
      // Select the appropriate input based on radio button values.
      // First time in...
      var j = $(".form-radios").find("input:radio").eq(0).attr('checked');
      if (j) selectCountry(); else selectIP();
      // Subsequent clicks...
      $(".form-radios").find("input:radio").click(function() {
        // As per Lyle, choosing to use click because of IE's stupid bug not to
        // trigger onChange until focus is lost.
        if ($(this).val() == 0) selectCountry(); else selectIP();
      });
      // When "Update" button is pushed, make an AJAX call to initiate the
      // database update without leaving this page.  Throw up a progress
      // marker because it takes a long time.
      $('#edit-update-database').click(function(){
        var databaseUpdated = function(data) {
          var result = $.parseJSON(data);
          $('#dbthrobber').removeClass('working').html(result['message'] + '  ' + result['count']).addClass('completed');
        }
        $('#dbthrobber').removeClass('message completed').addClass('working').html(Backdrop.t('Working...'));
        $.get(Backdrop.settings.basePath + 'admin/config/people/ip2country/update/' + $('#edit-ip2country-rir').val(), null, databaseUpdated);
        return false;
      });
      // When "Lookup" button is pushed, make an AJAX call to initiate the
      // database lookup.
      $('#edit-lookup-button').click(function(){
        var lookupCompleted = function(data) {
          var result = $.parseJSON(data);
          $('#lookup-message').html(result['message']);
        }
        $.get(Backdrop.settings.basePath + 'admin/config/people/ip2country/lookup/' + $('#edit-lookup').val(), null, lookupCompleted);
        return false;
      });
    }
  };
}) (jQuery);  // End local scope for $
