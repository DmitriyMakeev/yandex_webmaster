<?php

/**
 * @file
 * Admin page callbacks for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */

/**
 * Load idna_convert class for correct work with domain-names in punycode.
 */
require_once('idna_convert.class.php');

/**
 * Display top clicks and shows report for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster top queries'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  drupal_add_js(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.js');
  $form = array();
  // Get statistics.
  $stats = yandex_webmaster_get_top_clicks();
  if (isset($stats['error'])) {
    drupal_set_message(t('An error occured while getting clicks statistics. @error', array('@error' => filter_xss($stats['error']))), 'error');
    return $form;
  }
  // Display Yandex branding.
  global $language;
  $form['branding'] = array(
    '#type' => 'item',
    '#weight' => 100,
    '#markup' => '<div class="branding branding-' . $language->language . '">' . t('Data provided by !link service.', array('!link' => l(t('Yandex.Webmaster'), 'http://webmaster.yandex.com/'))) . '</div>',
  );

  // Empty statistics.
  if ($stats['total-clicks-count'] == 0) {
    $form['summary'] = array(
      '#type' => 'item',
      '#markup' => t('Summary click count for last seven days') . ': '
      . $stats['total-clicks-count'] . '<br />'
      . t('Summary shows count for last seven days') . ': '
      . $stats['total-shows-count'],
    );
    return $form;
  }

  $form['procent'] = array(
    '#type' => 'item',
    '#markup' => t('Percentage of shows count to the number of clicks') . ': ' . $stats['shows-clicks-procent'] . '%',
  );

  // Display top clicks list.
  $form['clicks_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary click count for last seven days') . ': '
      . $stats['total-clicks-count'] . '<br />'
      . t('Summary procent of popular query clicks') . ': '
      . $stats['top-clicks-percent'] . '%',
  );
  $form['clicks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Top clicks for last seven days'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#theme' => 'yandex_webmaster_top_table',
    '#header' => array(
      '#',
      t('Query'),
      t('Click count'),
      t('Position'),
      t('User query'),
    ),
  );
  foreach ($stats['clicks'] as $info) {
    $form['clicks'][] = array(
      '#tree' => TRUE,
      'query' => array('#markup' => $info['query']),
      'count' => array('#markup' => $info['count']),
      'position' => array('#markup' => $info['position']),
      'rank' => array('#markup' => $info['clicks-top-rank']),
      'custom' => array('#markup' => $info['is-custom']),
    );
  }

  // Display top shows list.
  $form['shows_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary shows count for last seven days') . ': '
      . $stats['total-shows-count'] . '<br />'
      . t('Summary procent of popular query shows') . ': '
      . $stats['top-shows-percent'] . '%',
  );
  $form['shows'] = array(
    '#type' => 'fieldset',
    '#title' => t('Top shows for last seven days'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#theme' => 'yandex_webmaster_top_table',
    '#header' => array(
      '#',
      t('Query'),
      t('Shows count'),
      t('Position'),
      t('User query'),
    ),
  );
  foreach ($stats['shows'] as $info) {
    $form['shows'][] = array(
      '#tree' => TRUE,
      'query' => array('#markup' => $info['query']),
      'count' => array('#markup' => $info['count']),
      'position' => array('#markup' => $info['position']),
      'rank' => array('#markup' => $info['shows-top-rank']),
      'custom' => array('#markup' => $info['is-custom']),
    );
  }
  return $form;
}

/**
 * Display last indexed pages report for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_last_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster last indexed pages'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  $form = array();
  // Get statistics.
  $stats = yandex_webmaster_get_last_index();
  if (isset($stats['error'])) {
    drupal_set_message(t('An error occured while getting last indexed pages statistics. @error', array('@error' => filter_xss($stats['error']))), 'error');
    return $form;
  }
  // Display Yandex branding.
  global $language;
  $form['branding'] = array(
    '#type' => 'item',
    '#weight' => 100,
    '#markup' => '<div class="branding branding-' . $language->language . '">' . t('Data provided by !link service.', array('!link' => l(t('Yandex.Webmaster'), 'http://webmaster.yandex.com/'))) . '</div>',
  );

  $host = variable_get('yandex_webmaster_basic_host_id', 0);
  $form['index_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary count of indexed pages') . ': '
      . $stats['index-count']
      . '<br />'
      . t('Indexed pages for last seven days') . ': '
      . count($stats['url']),
  );
  $form['view_all'] = array(
    '#type' => 'item',
    '#markup' => l(t('View all indexed pages'), 'http://webmaster.yandex.com/site/indexed-pages.xml?host=' . $host . '&path=*', array(
      'attributes' => array(
        'target' => '_blank',
        'title' => t('Open in new window'),
      ),
    )),
  );

  if (count($stats['url'])) {
    $idna = new idna_convert();
    // Display last indexed pages table.
    $form['url'] = array(
      '#type' => 'fieldset',
      '#title' => t('Indexed pages for last seven days'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#theme' => 'yandex_webmaster_list_table',
      '#header' => array(
        t('Address'),
      ),
    );
    foreach ($stats['url'] as $url) {
      $form['url'][] = array(
        '#tree' => TRUE,
        'value' => array('#markup' => urldecode($url)),
      );
    }
  }

  return $form;
}

/**
 * Display excluded from the index pages report for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_excluded_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster excluded pages'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  drupal_add_js(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.js');
  $form = array();
  // Get statistics.
  $stats = yandex_webmaster_get_excluded_pages();
  if (isset($stats['error'])) {
    drupal_set_message(t('An error occured while getting excluded pages statistics. @error', array('@error' => filter_xss($stats['error']))), 'error');
    return $form;
  }
  // Display Yandex branding.
  global $language;
  $form['branding'] = array(
    '#type' => 'item',
    '#weight' => 100,
    '#markup' => '<div class="branding branding-' . $language->language . '">' . t('Data provided by !link service.', array('!link' => l(t('Yandex.Webmaster'), 'http://webmaster.yandex.com/'))) . '</div>',
  );

  $form['excluded_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary count of excluded pages') . ': ' . $stats['url-errors'],
  );

  if (count($stats['url'])) {
    // Display excluded pages table.
    $form['url'] = array(
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#theme' => 'yandex_webmaster_excluded_table',
      '#header' => array(
        t('Severity'),
        t('Error type'),
        t('Count'),
        t('Actions'),
      ),
    );
    foreach ($stats['url'] as $info) {
      $form['url'][] = array(
        '#tree' => TRUE,
        'code' => array('#markup' => $info['code']),
        'count' => array('#markup' => $info['count']),
        'severity' => array('#markup' => $info['severity']),
      );
    }
  }

  return $form;
}

/**
 * Display external links report for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_report_links_form($form, $form_state) {
  drupal_set_title(t('Yandex.Webmaster external links'));
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  $form = array();
  // Get statistics.
  $stats = yandex_webmaster_get_external_links();
  if (isset($stats['error'])) {
    drupal_set_message(t('An error occured while getting external links statistics. @error', array('@error' => filter_xss($stats['error']))), 'error');
    return $form;
  }
  // Display Yandex branding.
  global $language;
  $form['branding'] = array(
    '#type' => 'item',
    '#weight' => 100,
    '#markup' => '<div class="branding branding-' . $language->language . '">' . t('Data provided by !link service.', array('!link' => l(t('Yandex.Webmaster'), 'http://webmaster.yandex.com/'))) . '</div>',
  );

  $host = variable_get('yandex_webmaster_basic_host_id', 0);
  $form['index_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary count of external links') . ': '
      . $stats['links-count']
      . '<br />'
      . t('External links for last seven days') . ': '
      . count($stats['url']),
  );
  $form['view_all'] = array(
    '#type' => 'item',
    '#markup' => l(t('View all external links'), 'http://webmaster.yandex.com/site/indexed/links.xml?host=' . $host . '&path=*', array(
      'attributes' => array(
        'target' => '_blank',
        'title' => t('Open in new window'),
      ),
    )),
  );

  if (count($stats['url'])) {
    $idna = new idna_convert();
    // Display last indexed pages table.
    $form['url'] = array(
      '#type' => 'fieldset',
      '#title' => t('External links for last seven days'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#theme' => 'yandex_webmaster_list_table',
      '#header' => array(
        t('Address'),
      ),
    );
    foreach ($stats['url'] as $url) {
      $form['url'][] = array(
        '#tree' => TRUE,
        'value' => array('#markup' => urldecode($url)),
      );
    }
  }

  return $form;
}

/**
 * Display configuration page for the Yandex.Webmaster module.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_config_form($form, $form_state) {
  drupal_add_css(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.css');
  drupal_add_js(drupal_get_path('module', 'yandex_webmaster') . '/yandex_webmaster.js');

  $client = yandex_webmaster_client();
  $form = array();
  // Check access token for expiration.
  yandex_webmaster_check_token_expiration();

  // Check security code.
  $code = variable_get('yandex_webmaster_code', '');
  if ($code != '') {
    $code_description = t('Security code was already set. If you need to get new code, !link.', array('!link' => l(t('click here'), yandex_webmaster_domain_name('oauth') . 'authorize?response_type=code&wizard=drupal7_yandex_webmaster_dylan_russain&client_id=' . $client->id . '&display=iframe', array('attributes' => array('target' => '_blank', 'rel' => 'modal')))));
    $code_prefix = '';
  }
  else {
    $code_prefix = '<div class="messages warning">' . t('Yandex.Webmaster module requires security code to work. !link, to get code.', array('!link' => l(t('Click here'), yandex_webmaster_domain_name('oauth') . 'authorize?response_type=code&wizard=drupal7_yandex_webmaster_dylan_russain&client_id=' . $client->id . '&display=iframe', array('attributes' => array('target' => '_blank', 'rel' => 'modal'))))) . '</div>';
    $code_description = '';
  }

  // Display Yandex branding.
  global $language;
  $form['branding'] = array(
    '#type' => 'item',
    '#markup' => '<div class="branding branding-' . $language->language . '">'
      . t('Data provided by !link service.', array('!link' => l(t('Yandex.Webmaster'), 'http://webmaster.yandex.com/')))
      . '<br/>'
      . l(t('Read user agreements'), 'http://legal.yandex.ru/webmaster_api/', array('attributes' => array('target' => '_blank')))
      . '</div>',
  );

  // Parse host list.
  $hostlist = yandex_webmaster_get_hosts();
  // Check for current host.
  $idna = new idna_convert();
  $current_host = $idna->decode($_SERVER['HTTP_HOST']);
  $current_host_id = yandex_webmaster_get_host_id($hostlist);

  // Security code field
  $form['yandex_webmaster_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Security code'),
    '#default_value' => variable_get('yandex_webmaster_code', ''),
    '#size' => 8,
    '#maxlength' => 8,
    '#prefix' => $code_prefix,
    '#description' => filter_xss($code_description),
  );
  // Statistics block
  $form['yandex_webmaster_block_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable statistics block'),
    '#default_value' => variable_get('yandex_webmaster_block_enabled', TRUE),
  );

  // Display all hosts
  $form['yandex_webmaster_full_list'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display full host list'),
    '#default_value' => variable_get('yandex_webmaster_full_list', TRUE),
  );

  // Verification type
  $form['yandex_webmaster_use_metatag'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use meta-tag on the front page for verification'),
    '#disabled' => $current_host_id != -1,
    '#default_value' => variable_get('yandex_webmaster_use_metatag', FALSE),
  );

  // Select language and domain name
  $form['yandex_webmaster_domain'] = array(
    '#type' => 'select',
    '#title' => t('Select Yandex.Webmaster domain name'),
    '#options' => array(
      'com' => t('Default') . ' (webmaster.yandex.com)',
      'ru' => t('Russian') . ' (webmaster.yandex.ru)',
    ),
    '#default_value' => variable_get('yandex_webmaster_domain', 'com'),
  );
  

  // Don't display other fields, if security code is empty.
  if (variable_get('yandex_webmaster_token', '') == '') {
    return system_settings_form($form);
  }
  // Empty host list.
  if (!count($hostlist)) {
    return system_settings_form($form);
  }
  // Display 'Add new host' message.
  if ($current_host_id == -1) {
    $form['add_new_host_message'] = array(
      '#type' => 'item',
      '#markup' => '<div class="messages warning">'
        . t('Current site (@site) is not in your host list. To add your site to Yandex.Webmaster service, !link.', array('@site' => $current_host, '!link' => l(t('click here'), 'admin/config/yandex_webmaster/add')))
        . '</div>',
    );
  }
  else {
    // Check for failed verification and sites in subfolders.
    if ($hostlist[$current_host_id]['verification'] == 'VERIFICATION_FAILED') {
      $uin = yandex_webmaster_get_host_uin($current_host_id);
      // Get uin for current host.
      if (!$uin) {
        drupal_set_message(t('An error occurred while getting current host UIN from Yandex.Webmaster service.'), 'error');
      }
      else {
        if (base_path() != '/') {
          $form['manual_verification_message'] = array(
            '#type' => 'item',
            '#markup' => '<div class="messages warning">'
              . t('Current site (@site) can\'t be automatic verified. You will need to create empty text file "@filename" in site root directory.', array('@site' => $current_host, '@filename' => 'yandex_' . $uin . '.txt'))
              . '</div>',
          );
        }
        elseif (variable_get('yandex_webmaster_verification_type', '') == 'META_TAG') {
          $form['manual_verification_message'] = array(
            '#type' => 'item',
            '#markup' => '<div class="messages warning">'
              . t('Unfortunately, Yandex.Webmaster service may not process meta-tag verification for the first time. To run check procedure once again, !link.', array('!link' => l(t('click here'), 'http://webmaster.yandex.com/site/verification.xml?wizard=verification&wizard-hostid=' . $current_host_id . '&host=' . $current_host_id, array(
                'attributes' => array(
                  'target' => '_blank',
                  'title' => t('Open in new window'),
                ),
                ))))
              . '</div>',
          );
        }
      }
    }
  }

  // Display hosts list.
  $basic_host = variable_get('yandex_webmaster_basic_host_id', 0);
  $form['hosts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Host list'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#theme' => 'yandex_webmaster_hosts_table',
    '#header' => array(
      t('Default'),
      t('Host name'),
      t('TCY'),
      t('URL count'),
      t('Index count'),
      t('Last access'),
      t('Actions'),
    ),
  );
  // Fill rows.
  $is_full_list = variable_get('yandex_webmaster_full_list', TRUE);
  foreach ($hostlist as $host_id => $host) {
    $actions = '';
    if ($host_id == $basic_host) {
      if ($host['url-count'] > 0) {
        $actions .= l(t('Open statistics'), 'admin/reports/yandex_webmaster', array(
          'attributes' => array(
            'class' => 'yandex_webmaster_chart',
            'title' => t('Open statistics'),
          ),
        ));
      }
    }
    if ($host_id == $current_host_id) {
      $actions .= l(t('Remove site'), 'admin/config/yandex_webmaster/remove', array(
        'attributes' => array(
          'class' => 'yandex_webmaster_remove',
          'title' => t('Remove site'),
        ),
      ));
    }
    if (!$is_full_list && $host_id != $current_host_id && $host_id != $basic_host) {
      continue;
    }
    $form['hosts'][$host_id] = array(
      '#tree' => TRUE,
      'name' => array('#markup' => $host['name']),
      'host_id' => array('#markup' => $host_id),
      'verification' => array('#markup' => $host['verification']),
      'crawling' => array('#markup' => $host['crawling']),
      'virused' => array('#markup' => $host['virused']),
      'last-access' => array('#markup' => $host['last-access']),
      'tcy' => array('#markup' => $host['tcy']),
      'url-count' => array('#markup' => $host['url-count']),
      'index-count' => array('#markup' => $host['index-count']),
      'actions' => array('#markup' => $actions),
      'enable' => array(
        '#type' => 'checkbox',
        '#title' => t('Set as default'),
        '#default_value' => $host_id == $basic_host,
      ),
    );
  }

  return system_settings_form($form);
}

/**
 * Validate yandex_webmaster_config_form form submissions and save access token,
 * if security code was changed.
 */
function yandex_webmaster_config_form_validate($form, &$form_state) {
  // Save basic host.
  foreach ($form_state['values'] as $key => $value) {
    if ($key > 0) {
      if (isset($value['enable']) && $value['enable'] == 1) {
        variable_set('yandex_webmaster_basic_host_id', (int) $key);
      }
    }
  }
  // Send request only when security code was changed.
  if ($form_state['values']['yandex_webmaster_code'] != variable_get('yandex_webmaster_code', '')) {
    $client = yandex_webmaster_client();
    $options = array(
      'method' => 'POST',
      'data' => 'grant_type=authorization_code&code=' . $form_state['values']['yandex_webmaster_code'] . '&client_id=' . $client->id . '&client_secret=' . $client->secret,
      'timeout' => 15,
      'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );
    $result = drupal_http_request(yandex_webmaster_domain_name('oauth') . 'token', $options);
    $json = drupal_json_decode($result->data);
    if ($result->code == '200') {
      // Save access token and his expiration date.
      variable_set('yandex_webmaster_token', $json['access_token']);
      variable_set('yandex_webmaster_token_expiration', time() + (int) $json['expires_in']);
      drupal_set_message(t('Security code was successfully checked.'));
      // Get Yandex.API user ID.
      $uid = yandex_webmaster_get_user_id();
      if ($uid != -1) {
        variable_set('yandex_webmaster_uid', $uid);
      }
    }
    else {
      // Clear previous settings.
      variable_set('yandex_webmaster_uid', '');
      variable_set('yandex_webmaster_basic_host_id', 0);
      variable_set('yandex_webmaster_token', '');
      variable_set('yandex_webmaster_token_expiration', '');
      // Display error message with description.
      drupal_set_message(t('Access request return error: @error. Try to get a new security code.', array('@error' => $json['error'] . ' (' . $result->code . ': ' . yandex_webmaster_state_description($result->status_message) . ')')), 'error');
    }
  }
}

/**
 * Display page for adding current site to Yandex.Webmaster service.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_add_site_form($form, $form_state) {
  if (variable_get('yandex_webmaster_code', '') == '') {
    drupal_set_message(t('Before adding new site to Yandex.Webmaster, you need to enter security code.'));
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  
  $uid = variable_get('yandex_webmaster_uid', '');
  $idna = new idna_convert();
  // Now we will encode hostname.
  $host = $idna->encode($_SERVER['HTTP_HOST']);

  // Send server request.
  $options = array(
    'method' => 'POST',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
    'data' => '<host><name>' . $host . '</name></host>',
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts', $options);
  // Host has sucessfully added to Yandex.Webmaster.
  if ($result->code == 201) {
    drupal_set_message(t('Site was sucessfully added to Yandex.Webmaster.'));
  }
  // If an error has occurred.
  else {
    preg_match("/<error>(.*?)<\/error>/", $result->data, $matches);
    $error = $matches[1];
    drupal_set_message(t('An error occurred while adding new site to Yandex.Webmaster. @error', array('@error' => $error)), 'error');
  }

  // Get current host id
  $hostlist = yandex_webmaster_get_hosts();
  // Empty host list.
  if (!count($hostlist)) {
    drupal_set_message(t('An error occurred while getting current host ID from Yandex.Webmaster service.'), 'error');
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  $host_id = '';
  // Now we will decode hostname.
  $host = $idna->decode($_SERVER['HTTP_HOST']);
  foreach ($hostlist as $id => $item) {
    if ($item['name'] == $host || str_replace('www.', '', $host) == str_replace('www.', '', $item['name'])) {
      $host_id = $id;
      break;
    }
  }
  if ($host_id == '') {
    drupal_set_message(t('An error occurred while getting current host ID from Yandex.Webmaster service.'), 'error');
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  variable_set('yandex_webmaster_basic_host_id', $host_id);

  // Get uin to add new site.
  $uin = yandex_webmaster_get_host_uin($host_id);
  if (!$uin) {
    drupal_set_message(t('An error occurred while getting current host UIN from Yandex.Webmaster service.'), 'error');
    drupal_goto('admin/config/services/yandex_webmaster');
  }

  // Check file access permission to create new file in site root directory
  // for verification.
  $root = $_SERVER['DOCUMENT_ROOT'];
  $use_meta_tag = variable_get('yandex_webmaster_use_metatag', FALSE);
  if (!$use_meta_tag) {
    // Create empty file and set verification type.
    $use_meta_tag = (!is_writable($root) || file_put_contents($root . 'yandex_' . $uin . '.txt', '') === FALSE);
  }
  // Drupal installation in subfolder.
  if (base_path() != '/') {
    drupal_set_message(t('Current site (@site) can\'t be automatic verified. You will need to create empty text file "@filename" in site root directory.', array('@site' => $current_host, '@filename' => 'yandex_' . $uin . '.txt')), 'error');
    $use_meta_tag = FALSE;
  }

  // Set verification type code.
  $verification_type = $use_meta_tag ? 'META_TAG' : 'TXT_FILE';
  variable_set('yandex_webmaster_verification_type', $verification_type);
  // Send PUT-request
  $options = array(
    'method' => 'PUT',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
    'data' => '<host><type>' . $verification_type . '</type></host>',
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts/' . $host_id . '/verify', $options);
  if ($result->code != 204) {
    drupal_set_message(t('An error occurred while sending verification type to Yandex.Webmaster service.'), 'error');
  }

  drupal_goto('admin/config/services/yandex_webmaster');
  return array();
}

/**
 * Remove current site from Yandex.Webmaster service.
 *
 * @ingroup yandex_webmaster
 */
function yandex_webmaster_remove_site_form($form, $form_state) {
  $form['text'] = array(
    '#type' => 'item',
    '#markup' => t('Are you sure want to remove current site from Yandex.Webmaster service?'),
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Remove'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );
  return $form;
}

/**
 * Submit procedure for removing current site from Yandex.Webmaster service.
 */
function yandex_webmaster_remove_site_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] != t('Remove')) {
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  if (variable_get('yandex_webmaster_code', '') == '') {
    drupal_set_message(t('Before adding new site to Yandex.Webmaster, you need to enter security code.'));
    drupal_goto('admin/config/services/yandex_webmaster');
  }

  $uid = variable_get('yandex_webmaster_uid', '');
  $idna = new idna_convert();
  $host = $idna->encode($_SERVER['HTTP_HOST']);

  $hostlist = yandex_webmaster_get_hosts();
  // Empty host list.
  if (!count($hostlist)) {
    drupal_set_message(t('An error occurred while sending delete request to Yandex.Webmaster service.'), 'error');
    drupal_goto('admin/config/services/yandex_webmaster');
  }
  $current_host_id = yandex_webmaster_get_host_id($hostlist);
  // Send DELETE-request.
  $options = array(
    'method' => 'DELETE',
    'headers' => array('Authorization' => 'OAuth ' . variable_get('yandex_webmaster_token', '')),
  );
  $result = drupal_http_request(yandex_webmaster_domain_name() . 'api/' . $uid . '/hosts/' . $current_host_id, $options);
  if ($result->code == 204) {
    drupal_set_message(t('Site was sucessfully removed from Yandex.Webmaster service.'));
  }
  else {
    drupal_set_message(t('An error occurred while sending delete request to Yandex.Webmaster service.'), 'error');
  }
  variable_set('yandex_webmaster_basic_host_id', 0);

  drupal_goto('admin/config/services/yandex_webmaster');
  return array();
}