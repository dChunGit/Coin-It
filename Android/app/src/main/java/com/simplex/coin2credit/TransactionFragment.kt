package com.simplex.coin2credit

import android.app.AlertDialog
import android.app.Dialog
import android.os.Bundle
import androidx.fragment.app.DialogFragment
import kotlinx.android.synthetic.main.dialog_tag.*

class TransactionFragment: DialogFragment() {
    companion object {
        fun newInstance(amount: Int): TransactionFragment = TransactionFragment().apply {
            arguments = Bundle().apply { putInt("amount", amount) }
        }
    }

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        val builder = AlertDialog.Builder(activity)
        val view = activity?.layoutInflater?.inflate(R.layout.dialog_tag, null)

        arguments?.getInt("amount")?.let { messageNum ->
            val second = when (messageNum%100) {
                in 0..9 -> String.format("0%d", messageNum%100)
                else -> String.format("%d", messageNum%100)
            }

            // TODO: use string resource template instead
            val outString = String.format("$%d.%s", messageNum/100, second)
            transfer.text = outString
        }

        builder.setView(view).setPositiveButton(R.string.dialog_acknowledge) { dialog, id -> }

        return builder.create()
    }
}