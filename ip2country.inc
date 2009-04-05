<?php
// $Id$

/**
 * @file
 * Utility routines to load the IP to Country database.
 *
 * Data is obtained from one of the five Regional Internet Registries
 * (AFRINIC, ARIN , APNIC, LACNIC, or RIPE).
 *
 * This code derived from "class eIp2Country" posted at
 * http://www.tellinya.com/read/2007/06/03/ip2country-translate-ip-address-to-country-code/
 */

/**
 * Update the database
 *
 * Truncates ip2country table then reloads from ftp servers
 *
 * @param $registry
 *   Regional Internet Registry from which to get the data.
 *   Allowed values are afrinic, apnic, arin (default), lacnic, or ripe.
 */
function ip2country_update_database($registry="arin") {
  $registry = drupal_strtolower(trim($registry));
  //
  // FTP files
  //
  $ftp_sites = array(
    "ftp://ftp.ripe.net/pub/stats/arin/delegated-arin-latest",
    "ftp://ftp.ripe.net/pub/stats/apnic/delegated-apnic-latest",
    "ftp://ftp.ripe.net/pub/stats/lacnic/delegated-lacnic-latest",
    "ftp://ftp.ripe.net/pub/stats/afrinic/delegated-afrinic-latest",
    "ftp://ftp.ripe.net/pub/stats/ripencc/delegated-ripencc-latest",
  );

  $entries   = 0;
  //
  // Set a run-time long enough so the script won't break
  //
  set_time_limit(10*60);  // 10 minutes!
  //
  // Truncate the table to remove old data
  //
  db_query("TRUNCATE TABLE {ip2country}");
  foreach ($ftp_sites as $ftp_file) {
    //
    // Replace Registry source with chosen registry
    //
    $ftp_file = str_replace("ftp.ripe", "ftp.". $registry, $ftp_file);
    //
    // RipeNCC is named ripe-ncc on APNIC registry
    //
    if ($registry == "apnic")
      $ftp_file = str_replace("stats/ripencc/", "stats/ripe-ncc/", $ftp_file);
    //
    // Fetch the FTP file using cUrl
    //
    $txt = _ip2country_fetch_page($ftp_file);
    //
    // Break the FTP file into lines
    //
    $lines = explode("\n", $txt);
    foreach ($lines as $line) {
      //
      // Trim each line for security
      //
      $line = trim($line, "\r\t \n");
      //
      // Split each line
      //
      $parts = explode("|", $line);
      if ($parts[2] != "ipv4" || count($parts) != 7)
        continue;
      //
      // Get the date of the allocation
      //
      $date_str   = $parts[5];
      $alloc_date = time();
      if (preg_match("/([0-9]{4})([0-9]{2})([0-9]{2})/", $date_str, $date_pcs))
        $alloc_date = mktime(0, 0, 0, $date_pcs[2], $date_pcs[3], $date_pcs[1]);
      //
      // The registry that owns the range
      //
      $owner_registry = $parts[0];
      //
      // Prepare the data for insert
      //
      $ip_start     = (int) ip2long($parts[3]);
      $ip_end       = (int) ip2long($parts[3])+($parts[4]-1);
      $range_length = (int) $parts[4];
      $country_code = $parts[1];
      //
      // Prepare the query
      //
      $sql = "INSERT INTO {ip2country} (
                `ip_range_first`,
                `ip_range_last`,
                `ip_range_length`,
                `country`,
                `registry`,
                `ip_range_date`
              )
              VALUES ( %d, %d, %d, '%s', '%s', FROM_UNIXTIME(%d) )";
      //
      // Insert range into the DB
      //
      db_query($sql,
               min($ip_start, $ip_end),
               max($ip_start, $ip_end),
               $range_length,
               $country_code,
               $owner_registry,
               $alloc_date
      );
      $entries++;
    }
  }
  //
  // Return record count in table
  //
  variable_set('ip2country_last_update', time());
  return $entries;
}


/**
 * Fetch the pages with cURL
 *
 * @param $url
 *   The ftp URL where the file is located
 */
function _ip2country_fetch_page($url) {
  $curl = curl_init();
  curl_setopt($curl, CURLOPT_URL, $url);
  curl_setopt($curl, CURLOPT_TIMEOUT, 60*5);
  curl_setopt($curl, CURLOPT_USERAGENT, "Drupal (+http://drupal.org/)");
  curl_setopt($curl, CURLOPT_HEADER, 0);
  curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, 0);
  $html = curl_exec($curl);
  curl_close($curl);
  return $html;
}
