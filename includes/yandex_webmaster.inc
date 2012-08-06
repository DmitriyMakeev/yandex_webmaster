<?php

/**
 * @file
 * Check functions and functions to parse the server response  for the
 * Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */

/**
 * Return Yandex.Webmaster domain name.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_domain_name($subdomain = 'webmaster') {
  $domain = variable_get('yandex_webmaster_domain', 'com');
  return 'https://' . $subdomain . '.yandex.' . $domain . '/';
}

/**
 * Get Yandex.API status description.
 *
 * @param $state
 *   Yandex.API state code.
 *
 * @param $context
 *   State code context for the correct interpretation of the states with
 *   the same code.
 *
 * @return string
 *   Full description of Yandex.API state code.
 */
function yandex_webmaster_state_description($state, $context = '') {
  switch ($state) {
    // Verification state.
    case 'IN_PROGRESS':
      return t('Verification in progress.');
    case 'NEVER_VERIFIED':
      return t('The site was never verified.');
    case 'VERIFICATION_FAILED':
      return t('Site verification has failed.');
    case 'VERIFIED':
      return t('Verified.');
    case 'WAITING':
      if ($context == 'verification') {
        return t('The site is in queue for verification.');
      }
      elseif ($context == 'index') {
        return t('The site expects the index.');
      }
      else {
        return t('The site is in queue.');
      }
    // Indexing state.
    case 'INDEXED':
      return t('Site was indexed.');
    case 'NOT_INDEXED':
      return t('Site was not indexed.');
    // Verification types.
    case 'AUTO':
      return t('Automatic verification.');
    case 'DNS_RECORD':
      return t('Adding TXT parameter in DNS-record of the site.');
    case 'HTML_FILE':
      return t('HTML file in the site root directory.');
    case 'MANUAL':
      return t('Manual verification.');
    case 'META_TAG':
      return t('Meta-tag in header of the front page.');
    case 'PDD':
      return t('Verification by Yandex.Mail for domains.');
    case 'TXT_FILE':
      return t('Text file in the site root directory.');
    case 'PDD_EXTERNAL':
      return t('Verification by external Yandex.Mail for domains.');
    case 'DELEGATION':
      return t('Access rights was delegated from another user.');
    case 'WHOIS':
      return t('Verification by information from the WHOIS service.');
    default:
      return $state;
  }
}

/**
 * Get Yandex.API code description.
 *
 * @param $state
 *   Yandex.API code.
 *
 * @return string
 *   Full description of Yandex.API code.
 */
function yandex_webmaster_code_description($code) {
  switch ($code) {
    // HTTP-code (redirect).
    case '300':
      return t('HTTP Status: Multiple Choices (300)');
    case '301':
      return t('HTTP Status: Moved Permanently (301)');
    case '302':
      return t('HTTP Status: Found (Moved Temporarily) (302)');
    case '303':
      return t('HTTP Status: See Other (303)');
    case '304':
      return t('HTTP Status: Not Modified (304)');
    case '305':
      return t('HTTP Status: Use Proxy (305)');
    case '307':
      return t('HTTP Status: Temporary Redirect (307)');
    // HTTP-code (client).
    case '400':
      return t('HTTP Status: Bad Request (400)');
    case '401':
      return t('HTTP Status: Unauthorized (401)');
    case '402':
      return t('HTTP Status: Payment Required (402)');
    case '403':
      return t('HTTP Status: Forbidden (403)');
    case '404':
      return t('HTTP Status: File Not Found (404)');
    case '405':
      return t('HTTP Status: Method Not Allowed (405)');
    case '406':
      return t('HTTP Status: Not Acceptable (406)');
    case '407':
      return t('HTTP Status: Proxy Authentication Required (407)');
    case '408':
      return t('HTTP Status: Request Timeout (408)');
    case '409':
      return t('HTTP Status: Conflict (409)');
    case '410':
      return t('HTTP Status: Gone (410)');
    case '411':
      return t('HTTP Status: Length Required  (411)');
    case '412':
      return t('HTTP Status: Precondition Failed (412)');
    case '413':
      return t('HTTP Status: Request Entity Too Large (413)');
    case '414':
      return t('HTTP Status: Request-URL Too Long (414)');
    case '415':
      return t('HTTP Status: Unsupported Media Type (415)');
    case '416':
      return t('HTTP Status: Requested Range Not Satisfiable (416)');
    case '417':
      return t('HTTP Status: Expectation Failed (417)');
    case '422':
      return t('HTTP Status: Unprocessable Entity (422)');
    case '423':
      return t('HTTP Status: Locked (423)');
    case '424':
      return t('HTTP Status: Failed Dependency (424)');
    case '426':
      return t('HTTP Status: Upgrade Required (426)');
    // HTTP-code (server).
    case '500':
      return t('HTTP Status: Internal Server Error (500)');
    case '501':
      return t('HTTP Status: Not Implemented (501)');
    case '502':
      return t('HTTP Status: Bad Gateway (502)');
    case '503':
      return t('HTTP Status: Bad Gateway (503)');
    case '504':
      return t('HTTP Status: Gateway Timeout (504)');
    case '505':
      return t('HTTP Status: HTTP Version Not Supported (505)');
    case '507':
      return t('HTTP Status: Insufficient Space to Store Resource (507)');
    case '510':
      return t('HTTP Status: Not Extended (510)');
    // Page load errors.
    case '1001':
      return t('Connection was closed');
    case '1002':
      return t('Document is too large');
    case '1003':
      return t('Document was denied by robots.txt');
    case '1004':
      return t('Document address does not correspond to the HTTP standard');
    case '1005':
      return t('Unsupported document format');
    case '1006':
      return t('DNS error');
    case '1007':
      return t('Status code does not correspond to standard HTTP');
    case '1008':
      return t('Wrong HTTP header');
    case '1010':
      return t('Unable to connect to the server');
    case '1013':
      return t('Wrong message length');
    case '1014':
      return t('Wrong encoding');
    case '1019':
      return t('Wrong number of data was submitted');
    case '1020':
      return t('HTTP header length exceeds the limit');
    case '1021':
      return t('Address length exceeds the limit');
    // Page parsing errors.
    case '2004':
      return t('Document contains meta-tag "refresh"');
    case '2005':
      return t('Document contains meta-tag "noindex"');
    case '2006':
      return t('Wrong encoding');
    case '2007':
      return t('This document is the server log');
    case '2010':
      return t('Wrong document format');
    case '2011':
      return t('Unable to detect encoding');
    case '2012':
      return t('Unsupported language');
    case '2014':
      return t('Document doesn\'t contains text');
    case '2016':
      return t('Too many links');
    case '2020':
      return t('Error extracting');
    case '2024':
      return t('Empty server response');
    case '2025':
      return t('Document is not canonical');
    default:
      return t('Error code #') . $code;
  }
}

/**
 * Check access token for expiration. If expiry date are close (1 week or
 * closer), user will get notification.
 */
function yandex_webmaster_check_token_expiration() {
  $date = variable_get('yandex_webmaster_token_expiration', '');
  if ($date != '' && $date <= time() - 604800) {
    // If token has already expired.
    if (time() >= $date) {
      drupal_set_message(filter_xss(t('Access token for Yandex.Webmaster module has expired. To update access token you need to get new security code on !link.', array('!link' => l(t('Yandex.Webmaster configuration page'), 'admin/config/services/yandex_webmaster')))), 'warning');
    }
    // If token expiry date are close.
    else {
      drupal_set_message(filter_xss(t('Access token for Yandex.Webmaster module will expire soon. To update access token you need to get new security code on !link.', array('!link' => l(t('Yandex.Webmaster configuration page'), 'admin/config/services/yandex_webmaster')))), 'warning');
    }
  }
}

/**
 * Get Yandex.API user ID.
 *
 * @return int
 *   Yandex.API user ID.
 */
function yandex_webmaster_get_user_id() {
  $options = array(
    'method' => 'GET',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/me', $options);
  // If server says "OK".
  if ($result->code == 200) {
    $uid = array_pop(explode('/', $result->redirect_url));
    // UID must be an integer value.
    if ($uid == (int) $uid) {
      return $uid;
    }
    else {
      drupal_set_message(t('An error occurred while checking Yandex.API user ID. Returned user ID are not an integer value.'), 'error');
      return -1;
    }
  }
  // An error has occurred.
  else {
    drupal_set_message(filter_xss(t('An error occurred while checking Yandex.API user ID for given access token. Your access token may be expired. To update access token you need to get new security code on !link.', array('!link' => l(t('Yandex.Webmaster configuration page'), 'admin/config/services/yandex_webmaster')))), 'error');
    return -1;
  }
}

/**
 * Get Yandex.Webmaster host uin.
 *
 * @param $host_id
 *   Yandex.API host ID.
 *
 * @return string|boolean
 *   Yandex.Webmaster host uin.
 */
function yandex_webmaster_get_host_uin($host_id) {
  $uid = variable_get('yandex_webmaster_uid', '');
  if ($uid == '') {
    return FALSE;
  }
  $options = array(
    'method' => 'GET',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts/' . $host_id . '/verify', $options);
  if ($result->code == 200) {
    preg_match("/<uin>(.*?)<\/uin>/", $result->data, $matches);
    if (isset($matches[1]) && $matches[1] != '') {
      variable_set('yandex_webmaster_uin', $matches[1]);
      return $matches[1];
    }
    return FALSE;
  }
  else {
    return FALSE;
  }
}

/**
 * Get Yandex.API hosts list.
 *
 * @return array
 *   Host list.
 */
function yandex_webmaster_get_hosts() {
  $list = array();
  $uid = variable_get('yandex_webmaster_uid', '');
  if ($uid == '') {
    return $list;
  }

  $options = array(
    'method' => 'GET',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts', $options);
  // Parse hosts list
  if ($result->code == 200) {
    $xml = simplexml_load_string($result->data);
    $hosts = $xml->xpath('//hostlist/host');
    foreach ($hosts as $host) {
      // Get host_id.
      $host_id = array_pop(explode('/', current($host->attributes()->{'href'})));
      $list[$host_id] = array();
      // Fill host data.
      // Some values can be empty, if current host are not in Yandex index.
      $list[$host_id]['name'] = current($host->{'name'});
      $list[$host_id]['verification'] = current($host->{'verification'}->attributes()->{'state'});
      $crawling = isset($host->{'crawling'}) ? current($host->{'crawling'}->attributes()->{'state'}) : '';
      $list[$host_id]['crawling'] = $crawling;
      $list[$host_id]['virused'] = isset($host->{'virused'}) ? current($host->{'virused'}) : 'false';
      $last_access = isset($host->{'last-access'}) ? strtotime(current($host->{'last-access'})) : t('Never');
      $list[$host_id]['last-access'] = $last_access;
      $list[$host_id]['tcy'] = current($host->{'tcy'});
      $list[$host_id]['url-count'] = current($host->{'url-count'});
      $list[$host_id]['index-count'] = current($host->{'index-count'});
    }
  }
  // An error has occurred.
  else {
    drupal_set_message(filter_xss(t('An error occurred while receiving Yandex.Webmaster hosts list. Your access token may be expired. To update access token you need to get new security code on !link.', array('!link' => l(t('Yandex.Webmaster configuration page'), 'admin/config/services/yandex_webmaster')))), 'error');
    return array();
  }
  return $list;
}

/**
 * Get current host ID.
 *
 * @param $hostlist
 *   Array returned by yandex_webmaster_get_hosts() function.
 *
 * @return int
 *   Host ID.
 */
function yandex_webmaster_get_host_id($hostlist) {
  $idna = new idna_convert();
  $current_host = $idna->decode($_SERVER['HTTP_HOST']);
  foreach ($hostlist as $host_id => $host) {
    // Check for same hostnames and hostnames without 'www' prefix.
    if ($host['name'] == $current_host || str_replace('www.', '', $current_host) == str_replace('www.', '', $host['name'])) {
      return $host_id;
    }
  }
  return -1;
}

/**
 * Get Yandex.API host stats.
 *
 * @param $host_id
 *   Yandex.API host ID.
 *
 * @return array
 *   Host stats.
 */
function yandex_webmaster_get_host_stats($host_id = 0) {
  $stats = array();
  $uid = variable_get('yandex_webmaster_uid', '');
  if ($uid == '') {
    $stats['error'] = t('Yandex.API UIN is undefined.');
    return $stats;
  }
  if ($host_id == 0) {
    $host_id = (int) variable_get('yandex_webmaster_basic_host_id', 0);
    if ($host_id == 0) {
      $stats['error'] = t('Basic host was not selected.');
      return $stats;
    }
  }
  $options = array(
    'method' => 'GET',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts/' . $host_id . '/stats', $options);
  // Parse hosts list
  if ($result->code == 200) {
    $host = simplexml_load_string($result->data);
    // Fill host stats.
    // Some values can be empty, if current host are not in Yandex index.
    $stats['name'] = current($host->name);
    $stats['verification'] = current($host->verification->attributes()->{'state'});
    $stats['verification_details'] = current($host->verification->details);
    $stats['crawling'] = isset($host->crawling) ? current($host->crawling->attributes()->state) : '';
    $stats['crawling_details'] = current($host->crawling->details);
    $stats['virused'] = current($host->virused);
    $stats['last-access'] = isset($host->{'last-access'}) ? strtotime(current($host->{'last-access'})) : t('Never');
    $stats['tcy'] = current($host->tcy);
    $stats['url-count'] = current($host->{'url-count'});
    $stats['url-errors'] = current($host->{'url-errors'});
    $stats['index-count'] = current($host->{'index-count'});
    $stats['internal-links-count'] = current($host->{'internal-links-count'});
    $stats['links-count'] = current($host->{'links-count'});
  }
  // An error has occurred.
  else {
    $xml = simplexml_load_string($result->data);
    $stats['error'] = current($xml->error);
  }
  return $stats;
}

/**
 * Get default Yandex.API query.
 *
 * @param $query
 *   Query value, like 'tops' or 'indexed'.
 *
 * @param $host_id
 *   Yandex.API host ID.
 *
 * @return array
 *   Result array.
 */
function yandex_webmaster_get_query($query, $host_id = 0) {
  $stats = array();
  $uid = variable_get('yandex_webmaster_uid', '');
  if ($uid == '') {
    $stats['error'] = t('Yandex.API UIN is undefined.');
    return $stats;
  }
  if ($host_id == 0) {
    $host_id = (int) variable_get('yandex_webmaster_basic_host_id', 0);
    if ($host_id == 0) {
      $stats['error'] = t('Basic host was not selected.');
      return $stats;
    }
  }
  $options = array(
    'method' => 'GET',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts/' . $host_id . '/' . $query, $options);
  if ($result->code == 200) {
    $stats['result'] = $result;
  }
  else {
    preg_match("/<error>(.*?)<\/error>/", $result->data, $matches);
    if (isset($matches[1]) && $matches[1] != '') {
      $stats['error'] = $matches[1];
    }
    else {
      $stats['error'] = $result->code;
    }
  }
  return $stats;
}

/**
 * Get Yandex.API top clicks.
 *
 * @param $host_id
 *   Yandex.API host ID.
 *
 * @return array
 *   Top clicks array.
 */
function yandex_webmaster_get_top_clicks($host_id = 0) {
  $stats = yandex_webmaster_get_query('tops', $host_id);
  if (isset($stats['error'])) {
    return $stats;
  }
  $result = $stats['result'];
  $xml = simplexml_load_string($result->data);
  // Fill host stats.
  $shows_count = current($xml->{'top-queries'}->{'total-shows-count'});
  $click_count = current($xml->{'top-queries'}->{'total-clicks-count'});
  $stats['total-shows-count'] = number_format($shows_count, 0, '', ' ');
  $stats['total-clicks-count'] = number_format($click_count, 0, '', ' ');
  // If no statistics.
  if ($shows_count == 0) {
    if ($shows_count == 0) {
      return $stats;
    }
    // Avoid devision by zero.
    $shows_count = 1;
  }
  $stats['shows-clicks-procent'] = number_format(100 - ($shows_count - $click_count) / $shows_count  * 100, 2, '.', ' ');
  $stats['top-shows-percent'] = number_format(current($xml->{'top-queries'}->{'top-shows-percent'}), 2, '.', ' ');
  $stats['top-clicks-percent'] = number_format(current($xml->{'top-queries'}->{'top-clicks-percent'}), 2, '.', ' ');
  $stats['shows'] = array();
  $stats['clicks'] = array();
  // Fill shows.
  $shows = $xml->xpath('//top-shows/top-info');
  foreach ($shows as $info) {
    $stats['shows'][] = array(
      'query' => current($info->query),
      'count' => (int) current($info->count),
      'position' => (int) current($info->position),
      'shows-top-rank' => (int) current($info->{'shows-top-rank'}),
      'is-custom' => current($info->{'is-custom'}) == 'false' ? t('No') : t('Yes'),
    );
  }
  // Fill clicks.
  $clicks = $xml->xpath('//top-clicks/top-info');
  foreach ($clicks as $info) {
    $stats['clicks'][] = array(
      'query' => current($info->query),
      'count' => (int) current($info->count),
      'position' => (int) current($info->position),
      'clicks-top-rank' => (int) current($info->{'clicks-top-rank'}),
      'is-custom' => current($info->{'is-custom'}) == 'false' ? t('No') : t('Yes'),
    );
  }
  return $stats;
}

/**
 * Get Yandex.API last indexed pages list.
 *
 * @param $host_id
 *   Yandex.API host ID.
 *
 * @return array
 *   Last indexed pages array.
 */
function yandex_webmaster_get_last_index($host_id = 0) {
  $stats = yandex_webmaster_get_query('indexed', $host_id);
  if (isset($stats['error'])) {
    return $stats;
  }
  $result = $stats['result'];
  $xml = simplexml_load_string($result->data);
  // Fill last indexed pages stats.
  $stats['index-count'] = number_format(current($xml->{'index-count'}), 0, '', ' ');
  $stats['url'] = array();
  $links = $xml->{'last-week-index-urls'}->url;
  $stats['url'] = array();
  foreach ($links as $url) {
    $stats['url'][] = current($url);
  }
  return $stats;
}

/**
 * Get Yandex.API last indexed pages list.
 *
 * @param $host_id
 *   Yandex.API host ID.
 *
 * @return array
 *   Last indexed pages array.
 */
function yandex_webmaster_get_excluded_pages($host_id = 0) {
  $stats = yandex_webmaster_get_query('excluded', $host_id);
  if (isset($stats['error'])) {
    return $stats;
  }
  $result = $stats['result'];
  $xml = simplexml_load_string($result->data);
  // Fill excluded pages stats.
  $stats['url-errors'] = number_format(current($xml->{'url-errors'}->attributes()->{'count'}), 0, '', ' ');
  $stats['url'] = array();
  $links = $xml->{'url-errors'}->{'url-errors-with-code'};
  foreach ($links as $url) {
    $stats['url'][] = array(
      'code' => current($url->attributes()->{'code'}),
      'count' => current($url->count),
      'severity' => current($url->severity),
    );
  }
  return $stats;
}

/**
 * Get Yandex.API external links list.
 *
 * @param $host_id
 *   Yandex.API host ID.
 *
 * @return array
 *   External links array.
 */
function yandex_webmaster_get_external_links($host_id = 0) {
  $stats = yandex_webmaster_get_query('links', $host_id);
  if (isset($stats['error'])) {
    return $stats;
  }
  $result = $stats['result'];
  $xml = simplexml_load_string($result->data);
  // Fill last indexed pages stats.
  $stats['links-count'] = number_format(current($xml->{'links-count'}), 0, '', ' ');
  $links = $xml->{'last-week-links'}->url;
  $stats['url'] = array();
  foreach ($links as $url) {
    $stats['url'][] = current($url);
  }
  return $stats;
}

/**
 * Get Yandex.Webmaster statistics and save it to the database.
 *
 * @param $hostlist
 *   Array returned by yandex_webmaster_get_hosts() function.
 */
function yandex_webmaster_update_stats($hostlist) {
  foreach ($hostlist as $host_id => $host) {
    if ($host['url-count'] > 0) {
      $stats = yandex_webmaster_get_host_stats($host_id);
      // Check for existed record.
      $count = db_select('yandex_webmaster_stats')
        ->condition('host_id', (int) $host_id)
        ->condition('timestamp', (int) $stats['last-access'])
        ->countQuery()
        ->execute()
        ->fetchField();
      if ($count == 0) {
        db_insert('yandex_webmaster_stats')
          ->fields(array(
            'host_id' => (int) $host_id,
            'timestamp' => (int) $stats['last-access'],
            'tcy' => (int) $stats['tcy'],
            'url_count' => (int) $stats['url-count'],
            'url_errors' => (int) $stats['url-errors'],
            'index_count' => (int) $stats['index-count'],
            'links_count' => (int) $stats['links-count'],
          ))
          ->execute();
      }
    }
  }
}

/**
 * Return Yandex branding form element.
 *
 * @param $show_agreements_link
 *   Boolean value to show agreements link.
 *
 * @return array
 *   Form element.
 */
function yandex_webmaster_branding_item($show_agreements_link = FALSE) {
  global $language;
  $agreements = $show_agreements_link ? '<br/>' . l(t('Read user agreements'), 'http://legal.yandex.ru/webmaster_api/', array('attributes' => array('target' => '_blank'))) : '';
  return array(
    '#type' => 'item',
    '#weight' => $show_agreements_link ? 0 : 100,
    '#markup' => '<div class="branding branding-' . $language->language . '">'
      . t('Data provided by !link service.', array('!link' => l(t('Yandex.Webmaster'), yandex_webmaster_domain_name(), array(
        'attributes' => array(
          'target' => '_blank',
          'title' => t('Yandex.Webmaster'),
        ),
        ))))
      . $agreements
      . '</div>',
  );
}

/**
 * Return site selector form element.
 *
 * @param $hostlist
 *   Array returned by yandex_webmaster_get_hosts() function.
 *
 * @return array
 *   Form element.
 */
function yandex_webmaster_selector_item($hostlist) {
  $options = array();
  $id = -1;
  foreach ($hostlist as $host_id => $host) {
    if ($host['crawling'] == 'INDEXED') {
      $options[$host_id] = $host['name'];
    }
    if ($host_id == arg(3)) {
      $id = $host_id;
    }
  }
  return array(
    '#tree' => TRUE,
    'options' => array(
      '#title' => t('Select site'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $id > 0 ? $id : variable_get('yandex_webmaster_basic_host_id', 0),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Update'),
    ),
    'save' => array(
      '#type' => 'checkbox',
      '#title' => t('Set as default'),
      '#default_value' => FALSE,
    ),
  );
}
