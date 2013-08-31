<?php

/**
 * @file
 * Utility routines to load the IP to Country database.
 *
 * Data is obtained from one of the five Regional Internet Registries
 * (AFRINIC, ARIN , APNIC, LACNIC, or RIPE).
 *
 * Data format is described by ftp://ftp.arin.net/pub/stats/arin/README
 *
 * This code derived from "class eIp2Country" posted at
 * http://www.tellinya.com/read/2007/06/03/ip2country-translate-ip-address-to-country-code/
 */


/**
 * Updates the database.
 *
 * Truncates ip2country table then reloads from ftp servers.
 *
 * @param $registry
 *   Regional Internet Registry from which to get the data.
 *   Allowed values are afrinic, apnic, arin (default), lacnic, or ripe.
 */
function ip2country_update_database($registry='arin') {
  $registry = drupal_strtolower(trim($registry));

  // FTP files.
  if ($registry == 'afrinic') {
    // afrinic only holds its own data - every other NIC has all the data.
    $ftp_urls = array(
      'ftp://ftp.ripe.net/pub/stats/afrinic/delegated-afrinic-latest',
    );
  }
  else {
    // Note arin doesn't play by the file-naming rules.
    $ftp_urls = array(
      'ftp://ftp.ripe.net/pub/stats/arin/delegated-arin-extended-latest',
      'ftp://ftp.ripe.net/pub/stats/apnic/delegated-apnic-latest',
      'ftp://ftp.ripe.net/pub/stats/lacnic/delegated-lacnic-latest',
      'ftp://ftp.ripe.net/pub/stats/afrinic/delegated-afrinic-latest',
      'ftp://ftp.ripe.net/pub/stats/ripencc/delegated-ripencc-latest',
    );
  }

  // Set a run-time long enough so the script won't break.
  drupal_set_time_limit(10*60);  // 10 minutes!

  // Truncate the table to remove old data.
  ip2country_empty_database();

  // Download data files from the chosen registry.
  $entries = 0;
  $summary_records = 0;
  foreach ($ftp_urls as $ftp_file) {
    // Replace Registry source with chosen registry.
    $ftp_file = str_replace('ftp.ripe', 'ftp.' . $registry, $ftp_file);

    // RipeNCC is named ripe-ncc on APNIC registry.
    if ($registry == 'apnic') {
      $ftp_file = str_replace('stats/ripencc/', 'stats/ripe-ncc/', $ftp_file);
    }

    // Fetch the FTP file using cURL.
    $txt = _ip2country_fetch_page($ftp_file);

    // Break the FTP file into records.
    $lines = explode("\n", $txt);
    unset($txt);  // Free up memory.

    // Loop over records.
    $summary_not_found = TRUE;
    foreach ($lines as $line) {
      // Trim each line for security.
      $line = trim($line, "\r\t \n");

      // Skip comment lines and blank lines.
      if (substr($line, 0, 1) == '#' || $line == '') {
        continue;
      }

      // Split record into parts.
      $parts = explode('|', $line);

      // We're only interested in the ipv4 records.
      if ($parts[2] != 'ipv4') {
        continue;
      }

      // Save number of ipv4 records from summary line.
      if ($summary_not_found && $parts[5] == 'summary') {
        $summary_not_found = FALSE;
        $summary_records += $parts[4];
        continue;
      }

      // The registry that owns the range.
      $owner_registry = $parts[0];

      // The country code for the range.
      $country_code = $parts[1];

      // Prepare the IP data for insert.
      $ip_start     = (int) ip2long($parts[3]);
      $ip_end       = (int) ip2long($parts[3])+($parts[4]-1);
      $range_length = (int) $parts[4];

      // Insert range into the DB.
      db_insert('ip2country')->fields(array(
                'ip_range_first'  => min($ip_start, $ip_end),
                'ip_range_last'   => max($ip_start, $ip_end),
                'ip_range_length' => $range_length,
                'country'         => $country_code,
                'registry'        => $owner_registry,
        )
      )->execute();
      $entries++;
    }
  }

  // Return record count in table.
  variable_set('ip2country_last_update', REQUEST_TIME);
  return $entries;
}


/**
 * Fetches the pages with cURL.
 *
 * @param $url
 *   The ftp URL where the file is located.
 *
 * @return
 *   FALSE if ftp fetch failed. Otherwise, a string containing the contents
 *   of the fetched file.
 */
function _ip2country_fetch_page($url) {
  $curl = curl_init();

  // Fetch requested file.
  curl_setopt($curl, CURLOPT_URL, $url);
  curl_setopt($curl, CURLOPT_TIMEOUT, 60*2);
  curl_setopt($curl, CURLOPT_USERAGENT, 'Drupal (+http://drupal.org/)');
  curl_setopt($curl, CURLOPT_HEADER, FALSE);
  curl_setopt($curl, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($curl, CURLOPT_FOLLOWLOCATION, TRUE);

  $html = curl_exec($curl);
  curl_close($curl);

  return $html;
}


/**
 * Empties the ip2country table in the database.
 */
function ip2country_empty_database() {
  db_truncate('ip2country')->execute();
}
