package com.simplex.coin2credit

import android.app.AlertDialog
import android.app.Dialog
import android.content.DialogInterface
import android.os.Bundle
import android.support.v4.app.DialogFragment
import android.widget.TextView

class TransactionFragment: DialogFragment() {
    companion object {
        fun newInstance(amount: Int): TransactionFragment {
            val transactionFragment = TransactionFragment()
            val args = Bundle()
            args.putInt("amount", amount)
            transactionFragment.arguments = args

            return transactionFragment
        }
    }

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        val builder = AlertDialog.Builder(activity)
        val inflater = activity?.layoutInflater
        val view = inflater?.inflate(R.layout.dialog_tag, null)

        val messageNum = arguments!!.getInt("amount")
        val second = when(messageNum%100) {
            in 0..9 -> String.format("0%d", messageNum%100)
            else -> String.format("%d", messageNum%100)
        }
        val outString = String.format("$%d.%s", messageNum/100, second)

        val amountView = view?.findViewById<TextView>(R.id.transfer)
        amountView?.text = outString

        builder.setView(view)
                .setPositiveButton(R.string.dialog_acknowledge, DialogInterface.OnClickListener { dialog, id -> run {
                    System.out.println("Test")
                    System.out.println("Test2")
                }
                })

        return builder.create()
    }
}