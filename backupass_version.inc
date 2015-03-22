<?php
function backupass_version()
{
  // Initialising output
  $output = '';
 
  // Table header
  $header = array(
    'fileid' => array('data' => 'File ID', 'field' => 'fileid'),
    'filename' => array('data' => 'File Name', 'field' => 'filename'),
    'filepath' => t('File Path'),
    'version' => array('data' => 'Version', 'field' => 'version'),
    'version_date' => array('data' => 'Version date', 'field' => 'file_createddate'),
  );
 
  // Setting the sort conditions
  if(isset($_GET['sort']) && isset($_GET['order'])) {
    // Sort it Ascending or Descending?
    if($_GET['sort'] == 'asc')
      $sort = 'ASC';
    else
      $sort = 'DESC';
 
    // Which column will be sorted
    switch($_GET['order']) {
      case 'File ID':
        $order = 'fileid';
        break;
      case 'File Name':
        $order = 'filename';
        break;
    case 'Version':
        $order = 'version';
        break;
    case 'Version date':
        $order = 'version_date';
        break;
      default:
        $order = 'fileid';
    }
  }
  else {
    $sort = 'ASC';
    $order = 'fileid';
  }
 
  // Query object
  $query = db_select("ba_filelist", "f");
  $query->leftJoin('ba_file_archive','a','f.fileid=a.fileid');
  // Adding fields
  $query->fields('f',array('fileid','filename','sourcepath',
            'parentpath','filepath','last_modified',
            'ext','filesize'));
  $query->fields('a',array('version','file_createddate','path'));
  // Set order by
  $query->orderBy($order, $sort);
 
  // Pagination
  $query = $query->extend('TableSort')->extend('PagerDefault')->limit(25);
    if (isset($_GET['fileid']) && $_GET['fileid'] != "") {  
           $query->condition('f.fileid',  $_GET['fileid'], '=');
       } 
  // Executing query
  //drupal_set_message($query);
  $result = $query->execute();
   
  // Looping for filling the table rows
  while($data = $result->fetchObject()) {
    // Adding the rows
    $rows[] = array($data->fileid, $data->filename,l($data->path,"backupass/getfile",array('query' => array('fileid' => $data->fileid, 'version' => $data->version))), $data->version, $data->file_createddate);
  }
  // Setting the output of the field
  $output = theme_table(
    array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array(),
      'sticky' => true,
      'caption' => '',
      'colgroups' => array(),
      'empty' => t("Table has no row!") // The message to be displayed if table is empty
    )
  ).theme('pager');
    if(isset($_SERVER['HTTP_REFERER']) && $_SERVER['HTTP_REFERER'] != "")
      $output.='<a href="'.$_SERVER['HTTP_REFERER'].'">Go back</a>';
    else
      $output.='<a href="backupass">Go back</a>';
  // Returning the output
  return $output;
}
