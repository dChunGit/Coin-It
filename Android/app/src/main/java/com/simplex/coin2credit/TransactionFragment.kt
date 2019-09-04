package com.simplex.coin2credit

import android.app.AlertDialog
import android.app.Dialog
import android.os.Bundle
import androidx.fragment.app.DialogFragment
import kotlinx.android.synthetic.main.dialog_tag.*
import java.text.NumberFormat

class TransactionFragment: DialogFragment() {
    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        val builder = AlertDialog.Builder(activity)
        val view = activity?.layoutInflater?.inflate(R.layout.dialog_tag, null)

        arguments?.getInt("amount")?.let { messageNum ->
            transfer.text = NumberFormat.getCurrencyInstance().format(messageNum/100.00)
        }

        builder.setView(view).setPositiveButton(R.string.dialog_acknowledge) { dialog, _ -> dialog.dismiss() }

        return builder.create()
    }

    companion object {
        fun newInstance(amount: Int): TransactionFragment = TransactionFragment().apply {
            arguments = Bundle().apply { putInt("amount", amount) }
        }
    }
}