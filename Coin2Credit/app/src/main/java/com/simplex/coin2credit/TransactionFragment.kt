package com.simplex.coin2credit

import android.app.AlertDialog
import android.app.Dialog
import android.content.DialogInterface
import android.os.Bundle
import android.support.v4.app.DialogFragment
import android.widget.TextView

class TransactionFragment: DialogFragment() {
    companion object {
        fun newInstance(amount: String): TransactionFragment {
            val transactionFragment = TransactionFragment()
            val args = Bundle()
            args.putString("amount", amount)
            transactionFragment.arguments = args

            return transactionFragment
        }
    }

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        val builder = AlertDialog.Builder(activity)
        val inflater = activity?.layoutInflater
        val view = inflater?.inflate(R.layout.dialog_tag, null)

        val message = arguments?.getString("amount")
        var outString = message

        if(message!!.contains(Regex("0-9"))) {
            val coins = message.toInt()
            val dollars = coins.div(100)
            val cents = coins.rem(100)
            outString = "$" + dollars + "." + cents
        }
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