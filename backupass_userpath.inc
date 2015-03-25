<?php
function backupass_userpath()
{
    
    $header = array(
        'user_path_id' =>t('ID'),
        'userid' => t('User id'),
        'path' => t('Path')   
	);
    
    $sql = db_select('ba_user_path', 'u')->extend('PagerDefault')->limit(25);
    
    $sql->fields('u',array('user_path_id','userid','path'));
    //$sql->extend('TableSort')->extend('PagerDefault')->limit(25); 
    $results = $sql->execute();
    
    $rows = array();
    
    foreach($results as $result){       
        $rows[] = array(      
            'user_path_id' => $result->user_path_id,
            //'filename' => $result->filename,
            'userid' => $result->userid,
            'path' => $result->path,//"version/?fieldid=".$result->fileid),
        );
    }
    
    $form['newuser'] = array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#title' => t('New user path'),
    );
    
    $form['newuser']['user'] = array(
            '#type' => 'textfield',
            '#title' => t('File name'),
            //'#default_value'=> isset($_GET['filter_name'])?$_GET['filter_name']:"",
            '#size' => 20,
            '#description' => t('User name'),
        );
    
    $form['newuser']['path'] = array(
            '#type' => 'textfield',
            '#title' => t('Path'),
            //'#default_value'=> isset($_GET['filter_name'])?$_GET['filter_name']:"",
            '#size' => 40,
            '#description' => t('eg.input "examplepath" to get '.variable_get('backupass_target_path').'/examplepath'),
        );
    
    $form['newuser']['submit_bt'] = array(
            '#type' => 'submit',
            '#value' => t('Save'),
        );
    
    $form['backass_action']['adminaction'] = array(
      '#type' => 'select',
      '#title' => 'Actions',
      '#options' => array(
          1 => t('Delete user')
      )
    );
 
    $form['backass_table'] = array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $rows,
        '#empty' => t('No user path found.'),
        '#attributes' => array('class' => array('back_ass'))
    ); 

    
    $form['pager'] = array('#markup' => theme('pager'));
    
    $form['submit'] = array(
        '#type' => 'submit', '#value'=>t('Submit')
    );
    return $form;
}

function backupass_userpath_submit($form, &$formstatus)
{
    dsm($formstatus);
    if(isset($formstatus["values"]["user"]) and $formstatus["values"]["user"] != ""){
        $formstatus["rebuild"] = true;
        $sql = db_select("ba_user_path","p")->fields("p",array('userid','path'))->condition("userid",$formstatus["values"]["user"],"=")->condition("path",$formstatus["values"]["path"],"=");
        $results = $sql->execute();
        if(!$results->rowCount())
        {
            db_insert("ba_user_path")->fields(array("userid"=>$formstatus["values"]["user"],"path"=>$formstatus["values"]["path"]))->execute();
        }
    }
}
