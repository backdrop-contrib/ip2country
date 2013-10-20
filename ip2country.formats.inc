<?php

/**
 * @file
 * Implementation of hook_date_formats() for the ip2country module.
 *
 * It doesn't make sense to keep this code in ip2country.module because
 * hook_date_formats() is only called when a module is enabled. Splitting
 * this code out into its own include file prevents it from being loaded
 * into memory on every page load.
 *
 * @author Tim Rohaly.    <http://drupal.org/user/202830>
 */


/**
 * Provides implementation for ip2country_date_formats().
 */
function ip2country_get_date_formats() {
  $formats = array();

  // Date-only formats.
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'n/j/Y',
    'locales' => array(),
  );
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'm/d/Y',
    'locales' => array(),
  );
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'd/m/Y',
    'locales' => array('en-gb', 'en-hk', 'en-ie', 'el-gr', 'es-es', 'fr-be', 'fr-fr', 'fr-lu', 'it-it', 'nl-be', 'pt-pt'),
  );
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'd.m.Y',
    'locales' => array('de-ch', 'de-de', 'de-lu', 'fi-fi', 'fr-ch', 'is-is', 'pl-pl', 'ro-ro', 'ru-ru'),
  );
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'Y-m-d',
    'locales' => array(),
  );
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'Y/m/d',
    'locales' => array('en-ca', 'fr-ca', 'no-no', 'sv-se'),
  );
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'M j Y',
    'locales' => array(),
  );
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'j M Y',
    'locales' => array(),
  );
  $formats[] = array(
    'type' => 'ip2country_date',
    'format' => 'Y M j',
    'locales' => array(),
  );

  // Time-only formats.
  $formats[] = array(
    'type' => 'ip2country_time',
    'format' => 'H:i:s T',
    'locales' => array('en-us', 'en-gb', 'en-hk', 'en-ie', 'el-gr', 'es-es', 'fr-be', 'fr-fr', 'fr-lu', 'it-it', 'nl-be', 'pt-pt', 'en-ca', 'fr-ca', 'no-no', 'sv-se', 'de-ch', 'de-de', 'de-lu', 'fi-fi', 'fr-ch', 'is-is', 'pl-pl', 'ro-ro', 'ru-ru'),
  );
  $formats[] = array(
    'type' => 'ip2country_time',
    'format' => 'H:i',
    'locales' => array(),
  );
  $formats[] = array(
    'type' => 'ip2country_time',
    'format' => 'g:ia',
    'locales' => array(),
  );

  return $formats;
}
