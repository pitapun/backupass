<?php
function backupass_userpath($form, $form_state)
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
 
    $form['backass_table'] = array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $rows,
        '#empty' => t('No user path found.'),
        '#attributes' => array('class' => array('back_ass'))
    ); 

    
    $form['pager'] = array('#markup' => theme('pager'));
    
    $form['submit1'] = array(
        '#type' => 'submit', '#value'=>t('Delete')
    );
    return $form;
}

function backupass_userpath_submit($form, &$formstatus)
{
    if(isset($formstatus["values"]["user"]) and $formstatus["values"]["user"] != ""){
        $formstatus["rebuild"] = true;
        $formstatus["values"]["path"] = "/".trim(trim($formstatus["values"]["path"], "/"),"\\");
        $sql = db_select("ba_user_path","p")->fields("p",array('userid','path'))->condition("userid",$formstatus["values"]["user"],"=")->condition("path",$formstatus["values"]["path"],"=");
        $results = $sql->execute();
        if(!$results->rowCount())
        {
            db_insert("ba_user_path")->fields(array("userid"=>$formstatus["values"]["user"],"path"=>$formstatus["values"]["path"]))->execute();
            drupal_set_message("Path added success.");
        }
        else
        {
            drupal_set_message("Path already exist","error");
        }
    }
    if(isset($formstatus["clicked_button"]["#value"]) and $formstatus["clicked_button"]["#value"] != "")
    {
        $formstatus["rebuild"] = true;
        if($formstatus["clicked_button"]["#value"] == "Delete")
        {
            $options = $formstatus["complete form"]["backass_table"]["#options"];
            //dsm($formstatus);
            foreach($formstatus["values"]["backass_table"] as $key=>$value)
            {
                if(is_string($value))
                {
                    drupal_set_message("User path id [".$options[$key]["user_path_id"]."] is deleted.");
                    $sql = db_delete("ba_user_path")
                            ->condition("user_path_id",$options[$key]["user_path_id"],"=");
                    $sql->execute();
                }
            }
        }
    }
}
