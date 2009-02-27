if (Drupal.jsEnabled) {
  $(document).ready(function() {
    // Remove focus from the "Update" button
    $('#uc-ip2country-admin-settings .form-submit').eq(1).focus();
    var selectCountry = function() {
        $('#edit-uc-ip2country-test-country').removeAttr('disabled');
        $('#edit-uc-ip2country-test-ip-address').attr('disabled', 'disabled');
    }
    var selectIP = function() {
        $('#edit-uc-ip2country-test-country').attr('disabled', 'disabled');
        $('#edit-uc-ip2country-test-ip-address').removeAttr('disabled');
    }
    // Select the appropriate input based on radio button values
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
    $('#edit-uc-ip2country-update-database').click(function(){
      var databaseUpdated = function(data) {
        var result = Drupal.parseJson(data);
        $('#dbthrobber').removeClass('working').html(result['message'] + '  ' + result['count']).addClass('completed');
      }
      $('#dbthrobber').removeClass('message completed').addClass('working').html('Working...&nbsp;&nbsp;&nbsp;&nbsp;');
      $.get(Drupal.settings['base_path'] + 'admin/store/settings/ip2country/update', null, databaseUpdated);
      return false;
    });
  });
}
