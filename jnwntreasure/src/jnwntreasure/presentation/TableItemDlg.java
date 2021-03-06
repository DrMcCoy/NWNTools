/*
 * TableItemDlg.java
 *
 * Created on May 1, 2004, 2:52 PM
 */

package jnwntreasure.presentation;

/**
 *
 * @author Jason Mazzotta
 */

/**
 * TBD
 */
public class TableItemDlg extends javax.swing.JDialog {
    
    /** Creates new form TableItemDlg */
    public TableItemDlg(java.awt.Frame parent, boolean modal) {
        super(parent, modal);
        initComponents();
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    private void initComponents() {//GEN-BEGIN:initComponents
        java.awt.GridBagConstraints gridBagConstraints;

        jLabel1 = new javax.swing.JLabel();
        itemTypeCombo = new javax.swing.JComboBox();
        okButton = new javax.swing.JButton();
        cancelButton = new javax.swing.JButton();

        getContentPane().setLayout(new java.awt.GridBagLayout());

        setTitle("Table Item Dialog");
        setResizable(false);

        jLabel1.setText("Action To Take");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 0.3;
        gridBagConstraints.weighty = 0.1;
        getContentPane().add(jLabel1, gridBagConstraints);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.weightx = 0.7;
        gridBagConstraints.weighty = 0.1;
        getContentPane().add(itemTypeCombo, gridBagConstraints);

        okButton.setText("OK");
        okButton.setPreferredSize(new java.awt.Dimension(75, 25));

        cancelButton.setText("Cancel");

        pack();
    }//GEN-END:initComponents
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton cancelButton;
    private javax.swing.JComboBox itemTypeCombo;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JButton okButton;
    // End of variables declaration//GEN-END:variables

    /**
     * TBD 
     */ 
    public javax.swing.JButton getOKButton()
    {
            return okButton;
    }
   
    /**
     * TBD
     */ 
    public javax.swing.JButton getCancelButton()
    {
            return cancelButton;
    }
   
    /**
     * TBD
     */ 
    public javax.swing.JComboBox getItemTypeCombo()
    {
            return itemTypeCombo;
    }
}
