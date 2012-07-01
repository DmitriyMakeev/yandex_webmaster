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
  $form['branding'] = yandex_webmaster_branding_item();

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
    '#markup' => t('Click count for last seven days') . ': '
      . $stats['total-clicks-count'] . '<br />'
      . t('Procent of popular query clicks') . ': '
      . $stats['top-clicks-percent'] . '%',
  );
  $form['clicks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Top clicks for last seven days'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $clicks = array();
  foreach ($stats['clicks'] as $info) {
    $clicks[] = array(
      $info['clicks-top-rank'],
      $info['query'],
      $info['count'],
      $info['position'],
      $info['is-custom'],
    );
  }
  $form['clicks']['table'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', array(
      'header' => array(
        '#',
        t('Query'),
        t('Click count'),
        t('Position'),
        t('User query'),
      ),
      'rows' => $clicks,
    ))
  );

  // Display top shows list.
  $form['shows_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Shows count for last seven days') . ': '
      . $stats['total-shows-count'] . '<br />'
      . t('Procent of popular query shows') . ': '
      . $stats['top-shows-percent'] . '%',
  );
  $form['shows'] = array(
    '#type' => 'fieldset',
    '#title' => t('Top shows for last seven days'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $shows = array();
  foreach ($stats['shows'] as $info) {
    $shows[] = array(
      $info['shows-top-rank'],
      $info['query'],
      $info['count'],
      $info['position'],
      $info['is-custom'],
    );
  }
  $form['shows']['table'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', array(
      'header' => array(
        '#',
        t('Query'),
        t('Shows count'),
        t('Position'),
        t('User query'),
      ),
      'rows' => $shows,
    ))
  );
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
  $form['branding'] = yandex_webmaster_branding_item();

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
    '#markup' => l(t('View all indexed pages'), yandex_webmaster_domain_name() . 'site/indexed-pages.xml?host=' . $host . '&path=*', array(
      'attributes' => array(
        'target' => '_blank',
        'title' => t('Open in new window'),
      ),
    )),
  );

  // Display last indexed pages table.
  if (count($stats['url'])) {
    $list = array();
    $counter = 0;
    foreach ($stats['url'] as $url) {
      ++$counter;
      $list[] = array(
        $counter,
        urldecode($url),
      );
    }
    $form['table'] = array(
      '#title' => t('Indexed pages for last seven days'),
      '#type' => 'markup',
      '#markup' => theme('table', array(
        'header' => array(
          '#',
          t('Address'),
        ),
        'rows' => $list,
      ))
    );
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
  $form['branding'] = yandex_webmaster_branding_item();

  $form['excluded_summary'] = array(
    '#type' => 'item',
    '#markup' => t('Summary count of excluded pages') . ': ' . $stats['url-errors'],
  );

  // Display excluded pages table.
  if (count($stats['url'])) {
    $list = array();
    $host = (int) variable_get('yandex_webmaster_basic_host_id', 0);
    foreach ($stats['url'] as $info) {
      $list[] = array(
        'data' => array(
          $info['severity'],
          yandex_webmaster_code_description($info['code']),
          $info['count'],
          l(t('View'), yandex_webmaster_domain_name() . 'site/errors/types.xml?host=' . $host . '&code=' . $info['code'], array(
            'attributes' => array(
              'target' => '_blank',
              'title' => t('Open in new window'),
            ),
          )),
        ),
        'class' => array(drupal_strtolower($info['severity'])),
      );
    }
    $form['table'] = array(
      '#title' => t('External links for last seven days'),
      '#type' => 'markup',
      '#markup' => theme('table', array(
        'header' => array(
          t('Severity'),
          t('Error type'),
          t('Count'),
          t('Actions'),
        ),
        'rows' => $list,
      ))
    );
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
  $form['branding'] = yandex_webmaster_branding_item();

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

  // Display last indexed pages table.
  if (count($stats['url'])) {
    $list = array();
    $counter = 0;
    foreach ($stats['url'] as $url) {
      ++$counter;
      $list[] = array(
        $counter,
        urldecode($url),
      );
    }
    $form['table'] = array(
      '#title' => t('External links for last seven days'),
      '#type' => 'markup',
      '#markup' => theme('table', array(
        'header' => array(
          '#',
          t('Address'),
        ),
        'rows' => $list,
      ))
    );
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

  $form['branding'] = yandex_webmaster_branding_item(TRUE);

  // Parse host list.
  $hostlist = yandex_webmaster_get_hosts();
  // Check for current host.
  $idna = new idna_convert();
  $current_host = $idna->decode($_SERVER['HTTP_HOST']);
  $current_host_id = yandex_webmaster_get_host_id($hostlist);

  // Security code field.
  $code = variable_get('yandex_webmaster_code', '');
  $form['code_field'] = array(
    '#title' => t('Security code'),
    '#type' => 'item',
  );
  $form['code_field']['yandex_webmaster_code'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('yandex_webmaster_code', ''),
    '#size' => 8,
    '#maxlength' => 8,
    '#prefix' => '<div class="container-inline form-item">',
  );
  $form['code_field']['button'] = array(
    '#type' => 'button',
    '#value' => $code != '' ? t('Refresh') : t('Get code'),
    '#suffix' => '</div>',
    '#attributes' => array(
      'class' => array('modal'),
      'rel' => yandex_webmaster_domain_name('oauth') . 'authorize?response_type=code&wizard=drupal7_yandex_webmaster_dylan_russain&client_id=' . $client->id . '&display=iframe',
      'onclick' => 'return false;',
    ),
  );

  // Statistics block.
  $form['yandex_webmaster_block_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable statistics block'),
    '#default_value' => variable_get('yandex_webmaster_block_enabled', TRUE),
  );

  // Display all hosts.
  $form['yandex_webmaster_full_list'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display full host list'),
    '#default_value' => variable_get('yandex_webmaster_full_list', TRUE),
  );

  // Select language and domain name.
  $form['yandex_webmaster_domain'] = array(
    '#type' => 'select',
    '#title' => t('Select Yandex.Webmaster domain name'),
    '#options' => array(
      'com' => t('Default') . ' (webmaster.yandex.com)',
      'ru' => t('Russian') . ' (webmaster.yandex.ru)',
      'ua' => t('Ukrainian') . ' (webmaster.yandex.ua)',
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
  // Check for failed verification and sites in subfolders.
  if ($current_host_id != -1 && $hostlist[$current_host_id]['verification'] == 'VERIFICATION_FAILED') {
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

  // Display hosts list.
  $basic_host = variable_get('yandex_webmaster_basic_host_id', 0);
  $header = array(
    'hostname' => t('Host name'),
    'tcy' => t('TCY'),
    'url_count' => t('URL count'),
    'index_count' => t('Index count'),
    'last_access' => t('Last access'),
    'actions' => t('Actions'),
  );
  $options = array();
  $default_value = array();

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

    // Get right hostname.
    $hostname = $host['name'];
    $idna = new idna_convert();
    $url = $idna->encode('http://' . $host['name']);
    // Display host state as icon.
    $state = $host['verification'];
    $virused = $host['virused'] == 'false' ? '' : ' VIRUSED';

    $options[$host_id] = array(
      '#attributes' => array(
        'class' => array('yandex_webmaster_host_row' . $virused),
      ),
      'hostname' => '<div class="yandex_webmaster_host_state ' . $state . $virused . '">'
        . '<div class="yandex_webmaster_hostname">' . $hostname
        . l($hostname, $url, array(
          'attributes' => array(
            'class' => 'yandex_webmaster_external_link',
            'target' => '_blank',
            'title' => t('Open in new window'),
          ),
        ))
        . '<br />'
        . '<span>'
        . yandex_webmaster_state_description($state, 'verification') . ' '
        . yandex_webmaster_state_description($host['crawling'], 'index')
        . '</span> '
        . '</div>',
      'tcy' => $host['tcy'],
      'url_count' => $host['url-count'] > 0 ? number_format($host['url-count'], 0, '', ' ') : $host['url-count'],
      'index_count' => $host['index-count'] > 0 ? number_format($host['index-count'], 0, '', ' ') : $host['index-count'],
      'last_access' => '<div class="yandex_webmaster_last_access">'
        . ($host['last-access'] > 0 ? t('@time ago', array('@time' => format_interval(time() - $host['last-access']))) . '<br /><span>' . format_date($host['last-access']) . '</span>' : $host['last-access'])
        . '</div>',
      'actions' => $actions,
    );
  }
  // Add current host to list, if it has not been added.
  if ($current_host_id == -1) {
    $options[$current_host] = array(
      'hostname' => '<div class="yandex_webmaster_host_state WAITING">'
        . '<div class="yandex_webmaster_hostname">' . $current_host . '<br />'
        . '<span>' . t('Not in your host list. Need to be added.') . '</span> '
        . '</div>',
      'tcy' => array(),
      'url_count' => array(),
      'index_count' => array(),
      'last_access' => array(),
      'actions' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('Add site'),
          '#href' => 'admin/config/yandex_webmaster/add',
          '#attributes' => array(
            'class' => array('yandex_webmaster_add_site'),
          ),
        ),
      ),
    );
  }
  // Display host list.
  $form['yandex_webmaster_basic_host_id'] = array(
    '#title' => 'Host list',
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#default_value' => $basic_host,
    '#multiple' => FALSE,
    '#empty' => t('Not one host was previously added.'),
  );
  return system_settings_form($form);
}

/**
 * Validate yandex_webmaster_config_form form submissions and save access token,
 * if security code was changed.
 */
function yandex_webmaster_config_form_validate($form, &$form_state) {
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
    if ($result->code == '200') {
      $json = drupal_json_decode($result->data);
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
      $error = '';
      if (isset($result->error)) {
        $error = $result->error;
      }
      elseif (isset($result->status_message)) {
        $error = yandex_webmaster_state_description($result->status_message);
      }
      drupal_set_message(t('Access request return error: @error. Try to get a new security code.', array('@error' => $result->code . ' (' . $error . ')')), 'error');
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
  // Verification type.
  $form['yandex_webmaster_use_metatag'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use meta-tag on the front page for verification'),
    '#disabled' => yandex_webmaster_get_host_id(yandex_webmaster_get_hosts()) != -1,
    '#default_value' => FALSE,
  );
  $form['actions'] = array(
    '#type' => 'tree',
    'submit' => array(
      '#type' => 'button',
      '#value' => t('Add site'),
    ),
  );

  return $form;
}
/**
 * Validate yandex_webmaster_add_site_form form submissions.
 */
function yandex_webmaster_add_site_form_validate($form, &$form_state) {
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
  $use_meta_tag = $form_state['values']['yandex_webmaster_use_metatag'];
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