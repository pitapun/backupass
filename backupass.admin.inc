<?php
function backupass_form_filter($form, $form_state)
{
    $disktotalspace = round(disk_total_space(variable_get('backupass_backup_path'))/1024/1024/1024,2);
    $diskfreespace = round(disk_free_space(variable_get('backupass_backup_path'))/1024/1024/1024,2);
    $precentage = round(($disktotalspace-$diskfreespace)*100/$disktotalspace,1);
    if($precentage < 80)
        drupal_set_message(theme_progress_bar(array("percent"=>$precentage, "message"=>"Backup disk used:".($disktotalspace-$diskfreespace)."/".$disktotalspace."GB"." ".$precentage."%")));
    else
        drupal_set_message(theme_progress_bar(array("percent"=>$precentage, "message"=>"Backup disk used:".($disktotalspace-$diskfreespace)."/".$disktotalspace."GB"." ".$precentage."%")),'warning');
    
    $form = array();
    $form['#method'] = 'get';
    $form['filter'] = array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#title' => t('Filter option')
    );
    $form['filter']['filter_ext'] = array(
        '#type' => 'textfield',
        '#title' => t('Extension'),
        '#default_value'=> isset($_GET['filter_ext'])?$_GET['filter_ext']:"",
        '#size' => 15,
    );
    
    $form['filter']['filter_name'] = array(
            '#type' => 'textfield',
            '#title' => t('File name'),
            '#default_value'=> isset($_GET['filter_name'])?$_GET['filter_name']:"",
            '#size' => 20,
        );
    
    $date_from = date('Y-m-d');
    $date_to = date('Y-m-d');
    
    if(isset($_GET['filter_date_from']['date']))
        $date_from = $_GET['filter_date_from']['date'];
    if(isset($_GET['filter_date_to']['date']))
        $date_to = $_GET['filter_date_to']['date'];
    
    $format = 'Y-m-d';
    $form['filter']['filter_date_from'] = array(
     '#type' => 'date_popup', // types 'date_text' and 'date_timezone' are also supported. See .inc file.
     '#title' => t('Date from'),
     '#default_value' => '', 
     '#date_format' => $format,
    );
    $form['filter']['filter_date_to'] = array(
     '#type' => 'date_popup', // types 'date_text' and 'date_timezone' are also supported. See .inc file.
     '#title' => t('Date from'),
     '#default_value' => '', 
     '#date_format' => $format,
    );
    
    $form['filter']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Filter'),
    );
    return $form;
}

function backupass_form($form, $form_state)
{
    global $user;
    $form = array();
	$prejob = false;
    if(isset($form_state['values']['adminaction']))
    {
    	if ($form_state['values']['adminaction'] == 1)
    	{
            $prejob = true;
            $query = "select * from (SELECT `fileid`,`filename`,COUNT(`fileid`) as `FileCount` FROM `ba_filelist` where deletedby='' GROUP BY `filename`+`last_modified`+`filesize` ) as a where a.filecount > 1";
            $pools = db_query($query);
            //$pools = $sql->execute();
            $rows = array();

            foreach($pools as $pool){    
                if($pool->filename != "")
                {
                    $sql = db_select('ba_filelist', 'f');
                    $sql->join('ba_file_version','v', 'f.fileid = v.fileid');
                    $sql->fields('f',array('fileid','filename','sourcepath',
                            'parentpath','filepath','last_modified',
                            'ext','filesize'))
                        ->fields('v',array('version'));
                    $sql->condition('filename', $pool->filename,'=');
                    $sql->condition('deletedby', '','=');
                    $results = $sql->execute();
                    foreach($results as $result)
                    $rows[] = array('fileid' => $result->fileid,
                    //'filename' => $result->filename,
                    'sourcepath' => $result->sourcepath,
                    'parentpath' => $result->parentpath,//"version/?fieldid=".$result->fileid),
                    'filepath' => l($result->filepath,"backupass/getfile",array('query' => array('fileid' => $result->fileid))),
                    'last_modified' => $result->last_modified,
                    'ext' => $result->ext,                                                                                                                                                                                                                           
                    'filesize' => $result->filesize,
                    'version' => l($result->version,"backupass/version",array('query' => array('fileid' => $result->fileid))),
                     );
                }
            }
	}
        else if ($form_state['values']['adminaction'] == 3)
        {
            //drupal_set_message("set delete here");
            
            //dsm($form_state);
            $options = $form_state["complete form"]["backass_table"]["#options"];
            foreach($form_state["values"]["backass_table"] as $key=>$value)
            {
                if(is_string($value))
                {
                    //dsm($options[$key]);
                    //drupal_set_message("You will delete the following files, do you confirm?");
                    //drupal_set_message($options[$key]["fileid"]);
                    $sql = db_update("ba_filelist")->fields(array('deletedby'=>$user->name,'deletedtime'=>date("Y-m-d H:i:s", time())))
                            ->condition("fileid",$options[$key]["fileid"],"=");
                    $sql->execute();
                }
            }
        }
		
        if($prejob)
        {
            $header = array(
            'fileid' =>t('File ID'),
            //'filename' => t('File name'),
            'sourcepath' => t('Backup path'),
            'parentpath' => array('data' => t('Parent path'), 'field'=>'parentpath'),
            'filepath' => array('data' => t('File name'), 'field'=>'filepath'),
            'last_modified' => array('data' => t('Last modified'), 'field'=>'last_modified'),
            'ext' => array('data' => t('Extension'), 'field'=>'ext'),
            'filesize' => t('File size'),
            'version' => t('Version')   
                    );

            $form = array();
            $form['backass_action']['adminaction'] = array(
                    '#type' => 'select',
                    '#title' => 'Actions',
                    '#options' => array(
                            1 => t('Find Duplicate'), 2=> t('Download Files'), 3=> t('Delete Files')
                    )
            );

            $form['backass_table'] = array(
              '#type' => 'tableselect',
              '#header' => $header,
              '#options' => $rows,
              '#empty' => t('No file found.'),
              '#attributes' => array('class' => array('back_ass'))
            ); 
            $form['submit'] = array(
                    '#type' => 'submit', '#value'=>t('Submit')
            );
            return $form;
        }
    }
    
    $header = array(
        'fileid' =>t('File ID'),
        //'filename' => t('File name'),
        'sourcepath' => t('Backup path'),
        'parentpath' => array('data' => t('Parent path'), 'field'=>'parentpath'),
        'filepath' => array('data' => t('File name'), 'field'=>'filepath'),
        'last_modified' => array('data' => t('Last modified'), 'field'=>'last_modified'),
        'ext' => array('data' => t('Extension'), 'field'=>'ext'),
        'filesize' => t('File size'),
        'version' => t('Version')   
	);
    
    $sql = db_select('ba_filelist', 'f')->extend('PagerDefault')->limit(25);
    $sql->join('ba_file_version','v', 'f.fileid = v.fileid');
    
    $sql->fields('f',array('fileid','filename','sourcepath',
            'parentpath','filepath','last_modified',
            'ext','filesize'))
        ->fields('v',array('version'));
    $sql->condition('f.deletedby', '','=');
    if (isset($_GET['filter_ext'])) {  
        $sql->condition('f.ext',  '%' . db_like($_GET['filter_ext']) . '%', 'LIKE');
    }   
     
    if (isset($_GET['filter_name'])) {  
        //$sql->condition('f.filepath',  '%' . db_like($_GET['filter_name']) . '%', 'LIKE');
        $sql->condition(db_or()->condition('f.filepath',  '%' . db_like($_GET['filter_name']) . '%', 'LIKE')->condition('f.parentpath',  '%' . db_like($_GET['filter_name']) . '%', 'LIKE'));
    }   
    
    if (isset($_GET['filter_date_from']['date']) && isset($_GET['filter_date_to']['date']) && $_GET['filter_date_from']['date'] != "") {  
        $sql->condition('f.last_modified',  array($_GET['filter_date_from']['date'],$_GET['filter_date_to']['date']), 'between');
    }   
    //$sql->condition('f.ext',  '%' . db_like('jpg') . '%', 'LIKE');
    if(isset($_GET["order"]) && isset($_GET["sort"]))
    {
        $order = "";
        if($_GET["order"] == "Extension")
            $order = "ext";
        else if($_GET["order"] == "Last modified")
            $order = "last_modified";
        else if($_GET["order"] == "Parent path")
            $order = "parentpath";
        else if($_GET["order"] == "File name")
            $order = "filepath";
        $sql ->orderBy($order,$_GET["sort"]);
    }
    else {
       $sql ->orderBy('sourcepath')
           ->orderBy('parentpath')
           ->orderBy('filepath');
    }

    //$sql->extend('TableSort')->extend('PagerDefault')->limit(25); 
    $results = $sql->execute();
    
    $rows = array();
    
    foreach($results as $result){       
        $rows[] = array(      
            'fileid' => $result->fileid,
            //'filename' => $result->filename,
            'sourcepath' => $result->sourcepath,
            'parentpath' => $result->parentpath,//"version/?fieldid=".$result->fileid),
            'filepath' => l($result->filepath,"backupass/getfile",array('query' => array('fileid' => $result->fileid))),
            'last_modified' => $result->last_modified,
            'ext' => $result->ext,                                                                                                                                                                                                                           
            'filesize' => $result->filesize,
            'version' => l($result->version,"backupass/version",array('query' => array('fileid' => $result->fileid))),
        );
    }
    
    $form['backass_action']['adminaction'] = array(
      '#type' => 'select',
      '#title' => 'Actions',
      '#options' => array(
          1 => t('Find Duplicate'), 2=> t('Download Files'), 3=> t('Delete Files')
      )
    );
 
    $form['backass_table'] = array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $rows,
        '#empty' => t('No file found.'),
        '#attributes' => array('class' => array('back_ass'))
    ); 

    $form['pager'] = array('#markup' => theme('pager'));
    
    $form['submit'] = array(
        '#type' => 'submit', '#value'=>t('Submit')
    );
    return $form;
}

function backupass_form_submit($form, &$form_state)
{
    $current_url =  $_SERVER['HTTP_HOST'] . request_uri();
    //dsm($form_state);
    $form_state['rebuild'] = TRUE;
}


