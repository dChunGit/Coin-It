package com.simplex.coin2credit

interface TransactionInterface {
    fun relayMessage(message: String)
    fun messageWritten(success: Int)
}